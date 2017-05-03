<!DOCTYPE html>
<html>

<head>

<title> Search </title>
</head>

<body>
<h1> Contents </h1>
<a href="addActorDirector.php">Add Actor/Director</a>&nbsp;
<a href="addMovie.php">Add Movie </a>&nbsp;
<a href="addActorMovie.php">Add Movie-Actor Relations </a>&nbsp;
<a href="addDirectorMovie.php">Add Movie-Director Relations </a>&nbsp;
<a href="search.php">Search</a>
<br/>
<br/>
<h1> Search </h1>

<form action= "search.php" method="GET">

<input type="text" name="search" ><br/>

<input type="submit" name="submit" value="Search">
</form>

<?php

if ($_GET["submit"]){
	$search = $_GET["search"];
	$search = mysql_escape_string($search);
	$search = trim($search);
	$array = explode(' ', $search);
   }

if ($search == ""){
	echo "Search input needed";
	}
else {
	$db = new mysqli('localhost', 'cs143', '', 'CS143');
    if($db->connect_errno > 0){
	    die('Unable to connect to database [' . $db->connect_error . ']');
       }

	echo "<h2> Related actors </h2>";
	
	$query = "SELECT id, last, first, dob FROM Actor WHERE (first LIKE '%$array[0]%' OR last LIKE '%$array[0]%')";
	for ($i = 1; $i < count($array); $i++){
		$query = $query." AND (first like '%$array[$i]%' OR last like '%$array[$i]%')";
	}

	if(!($rs = $db->query($query))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}	
	
	while ($row = $rs->fetch_assoc()){
		echo "<a href=\"showActorInfo.php?id=".$row["id"]."\">".$row["first"]." ".$row["last"]."(".$row["dob"].")</a><br/>"; 
	}
	
	echo "<h2> Related movies </h2>";
	$query2 = "SELECT id, title, year FROM Movie WHERE title LIKE '%$array[0]%'";
	for ($i = 1; $i < count($array); $i++){
		$query2 = $query2." AND title LIKE '%$array[$i]%'";
	}
	
	if(!($rs2 = $db->query($query2))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}	
	
	while ($row2 = $rs2->fetch_assoc()){
		echo "<a href=\"showMovieInfo.php?id=".$row2["id"]."\">".$row2["title"]." (".$row2["year"].")</a><br/>";
	}	
	
	echo "<br/><br/><br/>";
	$rs->free();
	$rs2->free();
	$db->close();
 }


?>

</body>
</html>