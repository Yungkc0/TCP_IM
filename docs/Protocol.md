This is an application-level protocol using TCP for IM with TEA encryption and a chat room for all logged clients.

1. DATA STRUCTURE
		
	1.1 Normal Data
  
	     0   1   3      5      7             27                       N
        +---+---+------+------+-------------+------------------------+
	    |cmd| n |fromID| toID |     rand    |          data          |
	    +---+---+------+------+-------------+------------------------+

        cmd is 1 byte command show type of a packet, it can be one of these:

         name       hex   from  to  data         description
        IM_SENDP    0x01   U1   U2  E(msg)       send text message
        LO_REQ      0x02   U    S   ---          login request
        LO_KEY      0x03   S    U   key          temporary private key to encrypt password
        LO_OK       0x04   S    U   E(ID)        give user who request to login an ID number
        IM_LIST     0x05   S    U   E(list)      list of other online users
        LO_PWD      0x06   U    S   E(pwd+name) encrypted password and user's name
				IM_GETLIST  0x07   U    S   ---          request for other users' list
        IM_LKEY     0x08   S    U   E(lkey)      private key of chat room
        IM_SENDL    0x09   U1   AU  E(msg)       send to chat room
        LO_ERR      0x0a   S    U   ---          there's some wrong with server
        IM_HEART    0x0b   U/S  U/S ---          heater packet
				IM_QUIT     0x0c   U    AU  ---          client quit
				IM_GETLKEY  0x0d   U    S   ---          request for key of chat room

		n is the length of data(equal to N-27) which must be integral multiple of 8, and maximum value of N is 4096 so that maximum length of data is 4096-27=4069 bytes.
		formID and toID are both 16 bit integer, specially S is server's number 0x00, AU is mean all other users number 0x01.
        rand is a 20 bytes list of 20 random numbers.
        data is a N-25 bytes string.
        E() means that content in bracket is encrypted.
				Maximum length of password and name are both 16 bytes.
        
	1.2 Special Data

      	1.2.1 list of other online users
        	27    29      45    47      62              N-1   N
        	+-----+-------+-----+-------+---------------+--+
            | ID1 | name1 | ID2 | name2 |  the rest   |\n|
            +-----+-------+-----+-------+---------------+--+
            
        1.2.2 heartbeat packet
        	5      6
            +------+
            | 0x7f |
            +------+
            
            There's no rand or data feild in this kind of packet.
        
2. LOGIN AND QUIT

	2.1 Login in

		U                                       S
            client send a login reqeust
         LO_REQ-------------------------------->
            server return a temporary private key 
         <----------------------------LO_KEY+key
            send encrypted password and name
         LO_PWD+E(pwd, name)------------------->
            login ok, give client an ID number
      1  <---------------------------LO_OK+E(ID)
			      or there's some wrong with server
      2  <--------------------------------LO_ERR

	2.2 Quit

		When recieve an IM_QUIT packet, server delete the source client's from client list.

3. SEND MESSAGE

	3.1 To Another Client

		U1                             S                        U2
           send text message
         IM_SENDP+E(msg)-------->
                    server decrypt message and encrypt again
                                        IM_SENDP+E(msg)-------->
		
	3.2 To Chat Room

		When first client logged in, server will return a TEA key for chat room and send the key to each later client.
        If all clients are quit, the key of chat room will be delete and wait for next "first client".
        
        Server forward an IM_SENDL packet to all online clients without decryption.
 
4. HEARTBEAT PACKET

		Client will send an IM_HEART packet per second to server, then server return a same packet.
        Server and client both set a timer and a variable cnt=0， then make cnt plus 1 per second. When recieve a heartbeat packet, set cnt equal to 0.
        If cnt is 5, program will be consider that the other side was broken.

5. ENCRYPT

		All data will be encrypted with TEA except temporary key in Logon Process.
        At first, client send password and name for this login, then server give a temporary random 256 bit key and client encrypt password and name using AES256 by the key.
        
        For IM_SENDP packet, client make a random number and make a 128 bit TEA key using the number and password by this:
                key = md5( strcat(rand, md5(password)))
        Then save the number in rand field of packet. When recieve an IM_SENDP packet, server will decrypt it with password and rand, then encrypt it again with toID's passowrd and rand.
        
        For IM_SENDL, client will encrypt and decrypt it with key of chat room and server only forward it.
        
        Because of TEA can only deal 8 bytes data once, data will be filled with '\0' so that length of data is integral multiple of 8 bytes.

6. ENCODING

		All is encoded using UTF-8.
