#include "wet.h"
#include <stdio.h>
#include "libpq-fe.h"

PGconn *conn;

int main(int argc, char* argv[]) {
	char connect_param[200];
	sprintf(connect_param,
		"host=csl2.cs.technion.ac.il dbname=%s user=%s password=%s",
		USERNAME, USERNAME, PASSWORD);
	conn = PQconnectdb(connect_param);
	
	if (!conn || PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to server failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return 1;
	}


	parseInput();
	return 0;
}

//TODO: Should we check if the table is empty?
void* addUser(const char*    name){
	char cmd[200];
	PGresult *res;
	sprintf(cmd, "INSERT INTO users SELECT MAX(id) + 1,\'%s\' FROM users", name);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}

	PQclear(res);

	sprintf(cmd, "SELECT id FROM users WHERE users.name =\'%s\'", name );
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	printf(ADD_USER, PQgetvalue(res,0,0));
}

//TODO: Find a more efficient algorithm
void* addUserMin(const char*    name){
	int min, i, n;
	char cmd[200];
	PGresult *res;
	
	res = PQexec(conn, "SELECT MIN(ID) FROM users");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}

	if (PQgetisnull(res, 0, 0))/*if table is empty*/{
		sprintf(cmd, "INSERT INTO users values(0, \'%s\')",name);
		res = PQexec(conn, cmd);
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			PQclear(res);
		}
		else{
			printf(USER_HEADER);
			printf(USER_RESULT, 0, name);
		}
	}
	else{
		min = atoi(PQgetvalue(res, 0, 0));
		PQclear(res);

		for (i = min + 1;; i++){
			sprintf(cmd, "INSERT INTO users values(%d, \'%s\')", i, name);
			res = PQexec(conn, cmd);
			if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
				PQclear(res);
			}
			else{
				printf(USER_HEADER);
				PQclear(res);
				res = PQexec(conn, "SELECT * FROM users ORDER BY id");
				n = PQntuples(res);
				for (i = 0; i < n; i++)
					printf(USER_RESULT, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
				break;
			}
		}
	}
	
}

//Could be improved
void* removeUser(const char*    id){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", id);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
	}
	else{
		sprintf(cmd, "DELETE FROM users WHERE id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);

		sprintf(cmd, "DELETE FROM photos WHERE user_id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);

		sprintf(cmd, "DELETE FROM tags WHERE user_id = %s", id);
		res = PQexec(conn, cmd);
		PQclear(res);
	}
}

void* addPhoto(const char*    user_id,
	const char*    photo_id){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", user_id);
	res = PQexec(conn,cmd);
	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
		return;
	}
	PQclear(res);

	sprintf(cmd, "SELECT * FROM photos WHERE id = %s AND user_id = %s", photo_id, user_id);
	res = PQexec(conn, cmd);
	if (PQntuples(res) > 0) {
		printf(EXISTING_RECORD);
		return;
	}
	PQclear(res);

	sprintf(cmd, "INSERT INTO photos VALUES(%s,%s)", photo_id, user_id);
	res = PQexec(conn,cmd);

}

void* tagPhoto(const char*    user_id,
	const char*    photo_id,
	const char*    info){
	char cmd[200];
	PGresult *res;

	sprintf(cmd, "SELECT * FROM users WHERE id = %s", user_id);
	res = PQexec(conn, cmd);
	if (PQntuples(res) == 0) {
		printf(ILL_PARAMS);
		return;
	}
	PQclear(res);

	sprintf(cmd, "SELECT * FROM tags WHERE photo_id = %s AND user_id = %s AND info = \'%s\'", photo_id, user_id, info);
	res = PQexec(conn, cmd);
	if (PQntuples(res) > 0) {
		printf(EXISTING_RECORD);
		return;
	}
	PQclear(res);

	sprintf(cmd, "INSERT INTO tags VALUES(%s,%s,\'%s\')", photo_id, user_id, info);
	res = PQexec(conn, cmd);

}

void* photosTags(){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT p.id, p.user_id, COUNT(info) AS tag_count FROM photos p LEFT OUTER JOIN tags t ON ( p.user_id = t.user_id AND p.id = t.photo_id ) GROUP BY p.id, p.user_id ORDER BY tag_count DESC, p.user_id ASC, p.id ASC");
	res = PQexec(conn, cmd);
	
	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(PHOTOS_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(PHOTOS_RESULT, PQgetvalue(res, i, 1), PQgetvalue(res, i, 0), PQgetvalue(res, i, 2));
	}
}

void* search(const char*    word){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT p.id, p.user_id, COUNT(info) AS tag_count FROM photos p LEFT OUTER JOIN tags t ON ( p.user_id = t.user_id AND p.id = t.photo_id ) WHERE t.info = \'%s\' GROUP BY p.id, p.user_id ORDER BY tag_count DESC, p.user_id ASC, p.id DESC", word);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(PHOTOS_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(PHOTOS_RESULT, PQgetvalue(res, i, 1), PQgetvalue(res, i, 0), PQgetvalue(res, i, 2));
	}
}

void* commonTags(const char*    k){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT info, count(*) AS photo_count FROM tags GROUP BY info HAVING count(DISTINCT photo_id) >= %s ORDER BY photo_count DESC, info ASC", k);
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(COMMON_HEADER);
		for (i = 0; i < PQntuples(res); i++)
			printf(COMMON_LINE, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
	}
}

//TODO: Some better solutions?
void* mostCommonTags(const char*    k){
	char cmd[200];
	PGresult *res;
	int i;

	sprintf(cmd, "SELECT info, count(*) AS photo_count FROM tags GROUP BY info ORDER BY photo_count DESC, info ASC");
	res = PQexec(conn, cmd);

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(COMMON_HEADER);
		for (i = 0; i < atoi(k); i++)
			printf(COMMON_LINE, PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
	}
}

void* similarPhotos(const char*    k,
	const char*    j){
	char cmd[500];
	PGresult *res, *que;
	int i;

	sprintf(cmd,
		"SELECT uid_1, pid_1 "
		"FROM ( "
		"		SELECT t1.user_id AS uid_1, t1.photo_id AS pid_1, t2.user_id AS uid_2, t2.photo_id AS pid_2 "
		"		FROM tags t1, tags t2 "
		"		WHERE t1.info = t2.info AND (t1.user_id <> t2.user_id OR t1.photo_id <> t2.photo_id) "
		"		GROUP BY t1.user_id, t1.photo_id, t2.user_id, t2.photo_id "
		"		HAVING COUNT(t1.info) >= %s "
		"	) AS t "
		"GROUP BY uid_1, pid_1 "
		"HAVING COUNT((UID_2, PID_2)) >= %s "
		"ORDER BY uid_1 ASC, pid_1 ASC ", 
		j, k);

	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}

	if (PQntuples(res) == 0)
		printf(EMPTY);
	else{
		printf(SIMILAR_HEADER);
		for (i = 0; i < PQntuples(res); i++){
			sprintf(cmd, "SELECT name FROM users WHERE id = %s", PQgetvalue(res,i,0));
			que = PQexec(conn, cmd);
			if (!que || PQresultStatus(que) != PGRES_TUPLES_OK) {
				fprintf(stderr, "Error executing commands: %s\n",
					PQresultErrorMessage(que));
				PQclear(que);
				return;
			}
			printf(SIMILAR_RESULT, PQgetvalue(res, i, 0), PQgetvalue(que, 0, 0), PQgetvalue(res, i, 1));
			PQclear(que);
		}
	}
}

void* autoPhotoOnTagOn(){
	PGresult *res;
	char cmd[5000];
	//PQexec(conn, "CREATE TRIGGER photo_trigger BEFORE INSERT ON tags FOR EACH ROW EXECUATE PROCEJURE add_photo();");

	sprintf(cmd,
		"CREATE OR REPLACE FUNCTION process_photos() RETURNS TRIGGER "
		"AS $$ "
		"BEGIN "
		"IF((NEW.photo_id, NEW.user_id) NOT IN(SELECT * FROM photos) AND NEW.user_id IN (SELECT users.id FROM users)) THEN "
				"INSERT INTO photos VALUES(NEW.photo_id, NEW.user_id); "
			"END IF; "
		"RETURN NEW; "
		"END; "
		"$$ LANGUAGE plpgsql;"
		);

	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	PQclear(res);

	res = PQexec(conn, "CREATE TRIGGER photo_trigger BEFORE INSERT ON tags FOR EACH ROW EXECUTE PROCEDURE process_photos();");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}

void* autoPhotoOnTagOFF(){
	PGresult *res;
	res = PQexec(conn, "DROP TRIGGER IF EXISTS photo_trigger ON tags");
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing commands: %s\n",
			PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}