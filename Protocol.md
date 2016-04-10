This is an application-level protocol for IM.

1. DATA STRUCTURE
  
	    0   1      3    5    13                   N
        +---+------+------+----------+--------------------+
	    |cmd|fromID| toID |   rand   |        data        |
	    +---+------+------+----------+--------------------+

	cmd
		Command of 
