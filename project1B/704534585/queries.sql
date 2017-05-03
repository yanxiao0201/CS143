
SELECT CONCAT(a.first,' ', a.last)
FROM Actor a, Movie m, MovieActor ma
WHERE m.title="Die Another Day" AND m.id=ma.mid AND a.id=ma.aid; 

#find the movie id, then find the actor id in the movie.Finally find the names of the actors in with the actor id

	 		 
SELECT COUNT(*)
FROM (SELECT DISTINCT aid
	  FROM MovieActor
	  GROUP BY aid
	  HAVING COUNT(mid) > 1)S; 

# first group the MovieActor table by actors, then choose those that have participate in more than one movies. Finally count the number of those participate in more than one movies. 


SELECT CONCAT(d.first,' ',d.last)
FROM Director d, MovieDirector md, MovieGenre mg
WHERE mg.genre='Horror' AND mg.mid=md.mid AND md.did=d.id; 

#select the name of directors who have directed horror movies before
