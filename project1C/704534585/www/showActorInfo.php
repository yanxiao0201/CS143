<!DOCTYPE html>
<html>

<head>

<title> Show Actor Information </title>
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
<h1> Show Actor Information </h1>

<?php

$aid = $_GET["id"];
if ($aid == ""){
	echo "Invalid actor ID";
	echo "<br/><br/><br/><br/>";
}
else {
	$db = new mysqli('localhost', 'cs143', '', 'CS143');
	if($db->connect_errno > 0){
		die('Unable to connect to database [' . $db->connect_error . ']');
		}
	$query = "SELECT last, first, sex, dob, dod FROM Actor WHERE id=$aid";
	
	if(!($rs1 = $db->query($query))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}
	
	$row = $rs1->fetch_assoc();
	echo "<b> Name: </b>".$row["first"]." ".$row["last"]."<br/>";
	echo "<b> Sex: </b>".$row["sex"]."<br/>";
	echo "<b> Date of Birth: <b/>".$row["dob"]."<br/>";
	if($row["dod"] != ""){
		echo "<b>Date of Death: </b> ".$row["dod"]."<br/><br/>"; 
	}
	else {
		echo "<b>Date of Death:</b> N/A";
	}

	echo "<h2> Movie Involved </h2>";


	$query2 = "SELECT MA.role, M.title, M.year, M.id FROM MovieActor MA, Movie M WHERE MA.aid=$aid AND MA.mid=M.id ORDER BY M.year DESC";

	if(!($rs2 = $db->query($query2))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}

	while ($row2= $rs2->fetch_assoc()){
		$link = "<a href=\"showMovieInfo.php?id=".$row2["id"]."\">".$row2["title"]." (".$row2["year"].")</a>";
		echo "\"".$row2["role"]."\" in ".$link."<br/>";
	}

	$rs1->free();
	$rs2->free();
	$db->close();
	
}


?>

</body>
</html>