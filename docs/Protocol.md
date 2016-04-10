This is an application-level protocol for IM with TEA encryption and a chat room for all logged clients.

1. DATA STRUCTURE
		
	1.1 Normal Data
  
	    0   1  3      5      7             27                       N
        +---+---+------+------+-------------+------------------------+
	    |cmd|n|fromID| toID |     rand    |          data          |
	    +---+---+------+------+-------------+------------------------+

        cmd is 1 byte command show type of a packet, it can be one of these:

         name       hex   from  to  data         description
        LO_REQ      0x02   U    S   ---          login request
        LO_KEY      0x03   S    U   key          temporary private key to encrypt password
        LO_OK       0x04   S    U   E(ID)        give user who request to login an ID number
        LO_PWD      0x06   U    S   E(pwd, name) encrypted password and user's name
        IM_LIST     0x05   S    U   E(list)      list of other online users
        IM_LKEY     0x08   S    U   E(lkey)      private key of chat room
        IM_QUIT     0x07   U    AU  E("quit")    tell other users "I quit" with encrypted string "quit"
        IM_SENDP    0x01   U1   U2  E(msg)       send text message
        IM_SENDL    0x09   U1   AU  E(msg)       send to chat room
        IM_ROOMKEY  0x0a   S    U   E(roomkey)   send key of chat room
        IM_HEART    0x0b   U    S   ---          heater packet

				n is the length of data(equal to N-27).
		formID and toID are both 16 bit integer, specially S is server's number 0x00, AU is mean all other users number 0xff.
        rand is a 20 bytes list of 20 random numbers.
        data is a N-25 bytes string.
        E() means that encrypt content in bracket.
        
	1.2 Special Data

      	1.2.1 list of other online users
        	25    41      61    77      97                 N
        	+-----+-------+-----+-------+------------------+
            | ID1 | name1 | ID2 | name2 |     the rest     |
            +-----+-------+-----+-------+------------------+
            
            User's name is a string which length less than 20 bytes.
            
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
         <---------------------------LO_OK+E(ID)
         
	2.2 Quit

		When recieve an IM_QUIT packet, server will forward it to chatroom and delete the source client's status, then all other clients will delete the client's status.

3. SEND MESSAGE

	3.1 To Another Client

		U1                             S                        U2
           send text message
         IM_SENDP+E(msg)-------->
                    server decrypt message and encrypt again
                                        IM_SENDP+E(msg)-------->
		
	3.2 To Chat Room

		When first client logged in, server will create an private key for chat room and send the key to each later client.
        If all clients are quit, the key of char room will be delete and wait for next "first client".
        
        Server forward IM_SENDL packet to all other online clients without decryption.
        
4. HEARTBEAT PACKET

		Client program will send a IM_HEART packet per second to server, then server return a same packet.
        Server and client both set a timer and a variable cnt=0， then make cnt plus 1 per second. When recieve a heartbeat packet, set cnt equal to 0.
        If cnt is 10, program will be consider that the other side was broken.
        
5. ENCRYPT

		All data will be encrypted with TEA except temporary key of Logon Process.
        At first, server give a temporary random 128 bit key and client use it to encrypt password and name.
        
        For IM_SENDP packet, client will make a random number and make a private key with the number and password use this:
                key = md5( strcat(rand, md5(password)))
        Then save the number in rand field of packet. When recieve an IM_SENDP packet, server will decrypt it with password and rand, then encrypt it again with toID's passowrd and rand.
        
        For IM_SENDL, client will encrypt and decrypt it with key of chat room and server only forward it.
        
        Because of TEA can only deal 8 bytes data once, data will be filled with '\0' so that length of data is integral multiple of 8 bytes. 

6. ENCODING

		All data is encoded using UTF-8.
