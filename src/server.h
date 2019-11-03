#ifndef SERVER_H_
#define SERVER_H_



class ESE532_Server{
public:

    //
	int setup_server();

	//
	int get_packet(unsigned char* buffer);
    
	
    // how many packets have we distributed
    int packet_counter;
    
    // how many packets did we break our file into?
    int table_size;
	
    // data read in from a file
    unsigned char* input_data;
	
    // table of pointers that points to an array where your packets live
    unsigned char** table;

protected:


};

#endif
