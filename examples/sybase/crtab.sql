drop table bookmarks
go
CREATE TABLE bookmarks (
  type CHAR(1) null,
  parent CHAR(255) null,
  path CHAR(255) null,
  name CHAR(50) null,
  url CHAR(100) null,
  owner CHAR(20) null,
  private CHAR(1) null
)
go

INSERT INTO bookmarks VALUES ('F','/Databases','/Databases/Mini_SQL','Mini_SQL','',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('F','/','//Databases','Databases','',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('F','','/Databases','Databases','',NULL,NULL)
go
INSERT INTO bookmarks VALUES ('B','/Databases/Mini_SQL','/Databases/Mini_SQL/Mini_SQL_Home_Page','Mini_SQL_Home_Page','http://AusWeb.com.au/computer/Hughes/msql/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/Databases/Mini_SQL','/Databases/Mini_SQL/Mini_SQL_Distribution','Mini_SQL_Distribution','ftp://Bond.edu.au/pub/Minerva/msql/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/Databases/Mini_SQL','/Databases/Mini_SQL/W3-mSQL_Home_Page','W3-mSQL_Home_Page','http://AusWeb.com.au/computer/Hughes/w3-msql/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/Databases/Mini_SQL','/Databases/Mini_SQL/W3-mSQL_Distribution','W3-mSQL_Distribution','ftp://Bond.edu.au/pub/Minerva/msql/w3-msql/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/Databases','/Databases/HTML_Translation_of_SQL-86','HTML_Translation_of_SQL-86','http://case50.ncsl.nist.gov/sql-86/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/Databases','/Databases/Database_Systems_Laboratory,_Univ_of_Massachusetts','Database_Systems_Laboratory,_Univ_of_Massachusetts','http://www-ccs.cs.umass.edu/db.html',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('F','/','//World-Wide_Web','World-Wide_Web','',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('F','/World-Wide_Web','/World-Wide_Web/HTTP_Servers','HTTP_Servers','',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/World-Wide_Web','/World-Wide_Web/Yahoo','Yahoo','http://www.yahoo.com/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/World-Wide_Web/HTTP_Servers','/World-Wide_Web/HTTP_Servers/Apache_HTTP_Server_Project','Apache_HTTP_Server_Project','http://www.apache.org/',NULL,NULL)
go

INSERT INTO bookmarks VALUES ('B','/World-Wide_Web/HTTP_Servers','/World-Wide_Web/HTTP_Servers/CERN_HTTP_server','CERN_HTTP_server','http://www.w3.org/hypertext/WWW/Daemon/Status.html',NULL,NULL)
go
