<!DOCTYPE html>
<html>

<head>

<title> Add Comments </title>
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
<h1> Add Comments </h1>

<form action= "addComment.php" method="GET">
<?php
$db = new mysqli('localhost', 'cs143', '', 'CS143');
if($db->connect_errno > 0){
	die('Unable to connect to database [' . $db->connect_error . ']');
  }
$mid = $_GET["mid"];

$namequery = "SELECT title, year FROM Movie WHERE id=$mid";
$rs1 = $db->query($namequery);
if(!($rs1 = $db->query($namequery))) {
	$errmsg = $db->error;
	echo "failed:".$errmsg."<br/>";
	exit(1);
	}

$row = $rs1->fetch_assoc();
$title = $row["title"];
$year = $row["year"];
$rs1->free();

?>

Movie:<br/> 
<select name="mid">
	<option value= "<?php echo $mid;?>" selected> <?php echo $title."(".$year.")";?> </option>
</select><br/>

Your Name: <br/>
<input type="text" name="name" maxlength="20" ><br/>

Rating:<br/>	
<select name="rating">
			<option value="5"> 5 </option>
			<option value="4"> 4 </option>
			<option value="3"> 3 </option>
			<option value="2"> 2 </option>
			<option value="1"> 1 </option>
		</select><br/>
Comments: <br/>
<textarea name="comment" cols="80" rows="10" ></textarea><br/>
<br/>

<input type="submit" name="submit" value="Add">
</form>

<?php

$comment = $name = $rating = "";

if($_GET["submit"]){
	$mid = $_GET["mid"];
	$name = trim($_GET["name"]);
    $rating = $_GET["rating"];
    $comment = trim($_GET["comment"]);
}
if($comment == ""){
	echo "Comments needed";
}
else{
	
	if($name == ""){
		$name = "Anonymous";
		}
	$name = mysql_escape_string($name);
	$comment = mysql_escape_string($comment);

	$query = "INSERT INTO Review VALUES('$name', now(), '$mid', '$rating', '$comment')";


	if(!($rs2 = $db->query($query))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	
	}

	echo "Review for Movie: ".$title." added successfully";

	$rs2->free();
	$db->close();

}


  
?>

</body>
</html>