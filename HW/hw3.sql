1,
(a) CHECK (weight <= 5) 
(b) 
CREATE TRIGGER laptopweight
AFTER INSERT ON Laptop
REFERENCING NEW ROW AS newrow
FOR EACH ROW
WHEN (newrow.weight	>5)
BEGIN
	UPDATE laptop
	SET weight = NULL
	WHERE weight > 5;
END

2, (1,8) (100,0) (100,0)

3, 
(a) 
CREATE VIEW EmployeeNames AS
	SELECT ename
	FROM Employee
	

CREATE VIEW DeptInfo AS
	SELECT dept, AVG(salary) avgsalary 
	FROM Employees
	GROUP BY dept

(b)
Mike has to have:
see EmployeeNames view
see  DeptInfo view
Delete on EmployeeNames

(c)
Yes. Mike will not be able to see the individual salary because he can only see the DeptInfo taht has average salary

(d)
UPDATE DeptInfo
SET avgsalary = 10000
WHERE dept = 'biology'

This fails because update on views cannot do with aggregates. 

(e)
GRANT ALL ON Employees, EmployeeNames
TO Joe WITH GRANT OPTION

(f)
REVOKE ALL ON Employees, EmployeeNames
FROM Joe CASCADE

GRANT ALL ON Employees, EmployeeNames
TO Joe

Everything about AllNames will be gone. But the StaffNames will remain since it is created by Joe.





