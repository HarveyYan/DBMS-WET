#include "wet.h"

int main(int argc, char* argv[]) {
	char connect_param[200];
	sprintf(connect_param,
		"host=csl2.cs.technion.ac.il dbname=%s user=%s password=%s",
		USERNAME, USERNAME, PASSWORD);
	PGconn *conn = PQconnectdb(connect_param);
	return 0;
}