<!DOCTYPE html>
<html>

<head>
<title> Add Movie-Actor Relations </title>
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
<h1> Add Movie-Actor Relations </h1>


<form action= "addActorMovie.php" method="GET">
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

$query2 = "SELECT id, first, last, dob FROM Actor ORDER BY first";
if(!($rs2 = $db->query($query2))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}


$actorOptions="";
while ($row = $rs2->fetch_assoc()){
	$aid=$row["id"];
	$first=$row["first"];
	$last=$row["last"];
	$dob=$row["dob"];
	$actorOptions.="<option value=\"$aid\">".$first." ".$last." (".$dob.")</option>";
}

$rs1->free();
$rs2->free();

?>


Movie:<br/> 
<select name="mid">
		<?=$movieOptions?>
</select><br/>
Actor: <br/>
<select name="aid">
		<?=$actorOptions?>
</select><br/>
Role:<br/>	
<input type="text" name="role" maxlength="50"><br/>
<br/>
<input type="submit" name="submit" value="Add">
</form>

<?php

$actorid = $movieid = $role = "";

if($_GET["submit"]){
	$actorid = $_GET["aid"];
    $movieid = $_GET["mid"];
    $role = trim($_GET["role"]);
}

if($role == ""){
	echo "Role needed";
}
else{
	$role = mysql_escape_string($role);
	
	$query3 = "INSERT INTO MovieActor VALUES('$movieid','$actorid','$role')";
	
	if(!($rs3 = $db->query($query3))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}

	echo "Relations Movie: ".$movieid." and Actor: ".$actorid." added successfully";

	$rs3->free();
}
	$db->close();
 
?>

</body>
</html>