INSERT INTO Movie VALUES(2, "Dadizhen", 2014, "PG-13","China");
#a movie already has id 2

INSERT INTO Movie VALUES(20000, "Dadizhen", 2018, "PG-13","China");
#future movie

INSERT INTO Actor VALUES(1, "Yan","Xiao","Male",2000-01-01,2000-01-01);
#an actor already has id 1

INSERT INTO Director VALUES(16, "Yan","Xiao",2000-01-01,2000-01-01);
#an director already has id 2

INSERT INTO MovieGenre VALUES(1,'Honor');
#No movie has id 1

INSERT INTO MovieGenre VALUES(-1,'Honor');
#movie id cannot be negative

INSERT INTO MovieDirector VALUES(2, -1);
#director cannot be negative

INSERT INTO MovieDirector VALUES(1, 1);
#No movie has id 1

INSERT INTO MovieDirector VALUES(3, 2);
#No director has id 2

INSERT INTO MovieActor VALUES(2, 2, "leading");
#No actor has id 2

INSERT INTO MovieActor VALUES(1, 1, "leading");
#No movie has id 1

INSERT INTO Review VALUES("Random", "01-01-2016 00:00:00", 1, 1, "Stupid");
#No movie has id 1

