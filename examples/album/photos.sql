#DROP TABLE photos;

CREATE TABLE photos (
  filename varchar(60) NOT NULL,
  album varchar(60),
  text varchar(120),
  PRIMARY KEY (filename)
);
