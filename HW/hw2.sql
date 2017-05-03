1.(a)
SELECT company-name
FROM Work
GROUP BY company-name
HAVING MIN(salary) > 100000

(b) 
company-name - company-name(salary <= 100000 Work)

(c) 
They give the same result in two different way. The first uses aggregates which can not
be represented by SQL. The second uses a except.

2.(a) 
SELECT person-name
FROM Work
GROUP BY person-name
HAVING SUM(salary) > ALL (SELECT SUM(salary)
						 FROM Work
						 GROUP BY person-name
						 HAVING person-name IN (SELECT person-name
						 						FROM Employee
						 						WHERE city = 'Los Angeles'))

SELECT person-name
FROM Work
GROUP BY person-name
HAVING SUM(salary) > (SELECT MAX(total)
                      FROM (SELECT person-name, SUM(salary) AS total
                            FROM Work
                            GROUP BY person-name) T
                            NATURAL JOIN Employee
                            WHERE city = 'Los Angeles')

(b)
SELECT person-name, SUM(salary) as total INTO ms
FROM Work w
GROUP BY person-name
HAVING w.person-name IN (SELECT manager-name
						 FROM Manage m)
						 
SELECT person-name, SUM(salary) as total INTO alls
FROM Work w
GROUP BY person-name


SELECT ms.person-name
FROM ms
WHERE ms.total > SOME ( SELECT total
						FROM alls
						WHERE person-name IN (SELECT person-name
											  FROM Manage
											  WHERE ms.person-name = manager-name
											  )
					   ) 
 						
3.(a) i. 
(SELECT name, address
 FROM MovieStar
 WHERE gender = 'F'
)
INTERSECT
(SELECT name, address
 FROM MovieExec
 WHERE newtWorth > 1000000
)

ii.
SELECT name, address
FROM MovieStar MS
NATURAL JOIN MovieExec ME
WHERE gender = 'F' AND netWorth > 1000000

(b) i.
(SELECT name
 FROM MovieStar
)
EXCEPT
(SELECT name
 FROM MovieExec
)

ii. 
SELECT MS.name
FROM MovieStar MS
LEFT OUTER JOIN MovieExec ME ON MS.name = ME.name
WHERE MS.name <> ME.name 

4.(a)
SELECT AVG(speed)
FROM Desktop

(b)
SELECT AVG(price)
FROM Laptop l
NATURAL JOIN ComputerProduct cp
WHERE weight < 2

(c)
SELECT AVG(price)
FROM ComputerProduct cp
WHERE manufacturer = "Dell"

(d)
SELECT AVG(price)
FROM Laptop l
GROUP BY speed

(e) 
SELECT manufacturer
FROM ComputerProduct cp
GROUP BY model
HAVING COUNT (*) > 2

5.(a)
INSERT INTO ComputerProduct
VALUES ("HP", 1200, 1000)

INSERT INTO Desktop
VALUES (1200, 1.2, 256, 80)

(b)
DELETE FROM ComputerProduct
WHERE manufacturer = "IBM" AND price < 1000

DELETE FROM Desktop
WHERE model NOT IN (SELECT model
					FROM ComputerProduct
				   )

(c)
UPDATE Laptop
SET weight = weight + 1
WHERE model IN (SELECT model
				FROM ComputerProduct
				WHERE manufacturer = "Gateway"
				)
















