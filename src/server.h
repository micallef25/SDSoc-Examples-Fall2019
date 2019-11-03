#ifndef SERVER_H_
#define SERVER_H_



class ESE532_Server{
public:

    //
	int setup_server();

	//
	int get_packet(unsigned char* buffer);

    int packet_counter;

    int table_size;

    unsigned char* input_data;

    unsigned char** table;

protected:


};

#endif
