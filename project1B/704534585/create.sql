CREATE TABLE Movie (id INT, title VARCHAR(100), year INT, rating VARCHAR(10), company VARCHAR(50), PRIMARY KEY(id), CHECK (year < 2017))ENGINE=INNODB;
#primary key:id; check: year<2017

CREATE TABLE Actor (id INT, last VARCHAR(20), first VARCHAR(20), sex VARCHAR(6), dob DATE, dod DATE, PRIMARY KEY(id))ENGINE=INNODB;
#primary key: id

CREATE TABLE Director (id INT, last VARCHAR(20),first VARCHAR(20), dob DATE, dod DATE, PRIMARY KEY (id))ENGINE=INNODB;
#primary key: id

CREATE TABLE MovieGenre(mid INT, genre VARCHAR(20), CHECK(mid > 0), FOREIGN KEY(mid) REFERENCES Movie(id)) ENGINE=INNODB;
#check: Mmid is not negative; RI: mid references Movie.id

CREATE TABLE MovieDirector(mid INT, did INT, CHECK(did > 0), FOREIGN KEY(mid) REFERENCES Movie(id),FOREIGN KEY(did) REFERENCES Director(id))ENGINE=INNODB;
#check: did is not negative; RI: mid references Movie.id; did references Director.id

CREATE TABLE MovieActor (mid INT, aid INT, role VARCHAR(50), FOREIGN KEY(mid) REFERENCES Movie(id), FOREIGN KEY(aid) REFERENCES Actor(id))ENGINE=INNODB;
#RI: mid references Movie.id, aid references Actor.id

CREATE TABLE Review (name VARCHAR(20), time TIMESTAMP, mid INT, rating INT, comment VARCHAR(500), FOREIGN KEY(mid) REFERENCES Movie(id))ENGINE=INNODB;
#RI: mid references Movie.id

CREATE TABLE MaxPersonID(id INT)ENGINE=INNODB;

CREATE TABLE MaxMovieID(id INT)ENGINE=INNODB;
