<!DOCTYPE html>
<html>

<head>

<title> Add Actor/Director </title>
</head>

<body>

<h1> Contents </h1>
<a href="addActorDirector.php">Add Actor/Director</a>&nbsp;
<a href="addMovie.php">Add Movie</a>&nbsp;
<a href="addActorMovie.php">Add Movie-Actor Relations</a>&nbsp;
<a href="addDirectorMovie.php">Add Movie-Director Relations</a>&nbsp;
<a href="search.php">Search</a>
<br/>
<br/>

<h1> Add Actor/Director </h1>

<form action= "addActorDirector.php" method="GET">

<input type="radio" name="type" value="Actor" checked> Actor<br/>
<input type="radio" name="type" value="Director"> Director<br/>
<br/>

First name:<br/>
<input type="text" name="firstname"><br/>
Last name:<br/>
<input type="text" name="lastname"><br/>
<br/>

<input type="radio" name="sex" value="male" checked> Male<br/>
<input type="radio" name="sex" value="female"> Female<br/>
<br/>

Date of Birth<br/>
<input type="text" name="dob"><br/>
i.e. 1988-03-01<br/>
Date of Die<br/>
<input type="text" name="dod"><br/>
i.e. 2100-01-01 or leave blank of N/A<br/>
<br/>

<input type="submit" name="submit" value="Add">
</form>


<?php

function test_input($data){
	$data = trim($data);
	$data = stripslashes($data);
	return $data;
	}

$type = $firstname = $lastname = $sex = $dob = $dod = "";


if ($_GET["submit"]){
	$type = $_GET["type"];
	$firstname = test_input($_GET["firstname"]);
	$lastname = test_input($_GET["lastname"]);
	$sex = $_GET["sex"];
	$dob = test_input($_GET["dob"]);
	$dod = test_input($_GET["dod"]);
}		

$newdob = date_parse($dob);
$newdod = date_parse($dod);

#echo $type.$firstname.$lastname.$sex.$dob.$dod;

if ($firstname == "" || $lastname == ""){
	echo "Valid Name required";
	}
else if (preg_match('/[^A-Za-z\s\'-]/', $firstname) || preg_match('/[^A-Za-z\s\'-]/', $lastname)){
	echo "Invalid Name";
	}
else if ($dob == "" || !checkdate($newdob["month"],$newdob["day"],$newdob["year"])){
	echo "Valid Date of Birth required";
	}
else if ($dod != "" && !checkdate($newdod["month"],$newdod["day"],$newdod["year"])){
	echo "Valid Date of Die required or leave it blank";
	}

else{
	$db = new mysqli('localhost', 'cs143', '', 'CS143');
	if($db->connect_errno > 0){
		die('Unable to connect to database [' . $db->connect_error . ']');
		}
		
	$maxpIDquery = "SELECT MAX(id) FROM MaxPersonID";
	//echo "success";
	
	if(!($rs1 = $db->query($maxpIDquery))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
		}
	
	$row = $rs1->fetch_assoc();
	$maxpID = $row['MAX(id)'];
	$newmaxpID = $maxpID + 1; 
	
	$lastname = mysql_escape_string($lastname);
	$firstname = mysql_escape_string($firstname);
	
	if ($type == "Actor"){
		if ($dod == ""){
			$query = "INSERT INTO Actor VALUES('$newmaxpID','$lastname','$firstname','$sex','$dob',NULL)";
			}
		else{
			$query = "INSERT INTO Actor VALUES('$newmaxpID','$lastname','$firstname','$sex','$dob','$dod')";
			}
		}
		
	else{
		if ($dod == ""){
			$query = "INSERT INTO Director VALUES('$newmaxpID','$lastname','$firstname','$dob',NULL)";
			}
		else{
			$query = "INSERT INTO Director VALUES('$newmaxpID','$lastname','$firstname','$dob','$dod')";
			}
		}
	

	if(!($rs2 = $db->query($query))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}

	
	$update = "UPDATE MaxPersonID SET id=$newmaxpID WHERE id=$maxpID";

	if(!($rs3 = $db->query($update))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}
	

	echo "New ".$type." addded with ID: ".$newmaxpID;
	
	
    $rs1->free();
    $rs2->free();
    $rs3->free();

	$db->close();
		
}


?>

</body>
</html>