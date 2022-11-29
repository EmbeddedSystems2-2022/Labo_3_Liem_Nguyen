#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include <json.h>
#include <arpa/inet.h>

int 
main ()
{
    MYSQL_RES* result;
    MYSQL_ROW row;
    MYSQL *con = mysql_init(NULL);

    printf ("%s%c%c\n","Content-Type:text/html;charset=iso-8859-1",13,10);
    printf ("<h1>EMBEDDED SYSTEMS - LABO 3</h1><br><br>\n");

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "liem", "liem123", "labo3", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    mysql_query(con,"SELECT * from labo3;");
    result = mysql_store_result(con);

    printf("<table><tr><th>Time</th><th>Data</th></tr>");
    while((row = mysql_fetch_row(result)))
    {
        printf("<script>console.log(\"%s : %s\");</script>",row[0],row[1]);
        printf("<tr><th>%s</th><th>%s</th></tr>",row[0],row[1]);
    }
    printf("</table>");
    printf("<style>table{width: 100%%;}td,th{border: 1px solid #dddddd;text-align: left;padding: 8px;}</style>");

    mysql_close(con);

    return 0;
}