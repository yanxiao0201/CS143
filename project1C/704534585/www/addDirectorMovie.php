<!DOCTYPE html>
<html>

<head>
<title> Add Director-Movie </title>
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

<h1> Add Director-Movie </h1>


<form action= "addDirectorMovie.php" method="GET">

<?php
$db = new mysqli('localhost', 'cs143', '', 'CS143');
if($db->connect_errno > 0){
	 die('Unable to connect to database [' . $db->connect_error . ']');
}

$query = "SELECT id, title, year FROM Movie ORDER BY title";
if(!($rs1 = $db->query($query))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}
	
$movieOptions="";
while ($row = $rs1->fetch_assoc()){
	$mid=$row["id"];
	$title=$row["title"];
	$year=$row["year"];
	$movieOptions.="<option value=\"$mid\">".$title." (".$year.")</option>";
}

$query2 = "SELECT id, first, last, dob FROM Director ORDER BY first";
if(!($rs2 = $db->query($query2))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}


$directorOptions="";
while ($row = $rs2->fetch_assoc()){
	$did=$row["id"];
	$first=$row["first"];
	$last=$row["last"];
	$dob=$row["dob"];
	$directorOptions.="<option value=\"$did\">".$first." ".$last." (".$dob.")</option>";
}

$rs1->free();
$rs2->free();

?>


Movie:<br/> 
<select name="mid">
		<?=$movieOptions?>
</select><br/>
Director: <br/>
<select name="did">
		<?=$directorOptions?>
</select><br/>

<input type="submit" name="submit" value="Add">
</form>

<?php

$directorid = $movieid = "";

if($_GET["submit"]){
	$directorid = $_GET["did"];
    $movieid = $_GET["mid"];
}

if($directorid =="" || $movieid ==""){
	echo "Director/Movie info needed";
}
else{

	$query = "INSERT INTO MovieDirector VALUES('$movieid','$directorid')";
	
	if(!($rs3 = $db->query($query))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}

	echo "Relations Movie: ".$movieid." and Director: ".$directorid." added successfully";

	$rs3->free();
	$db->close();
  }
  
?>

</body>
</html>