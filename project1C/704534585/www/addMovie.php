<!DOCTYPE html>
<html>

<head>

<title> Add Movie </title>
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

<h1> Add Movie </h1>

<form action= "addMovie.php" method="GET">


Title:<br/>
<input type="text" name="title"><br/>
Release Year:<br/>
<input type="text" name="year" maxlength="4"><br/>
MPAA rating:<br/>
<select name="mpaarating">
				<option value="G" >G</option>
				<option value="NC-17" >NC-17</option>
				<option value="PG" >PG</option>
				<option value="PG-13" >PG-13</option>
				<option value="R" >R</option>
				<option value="surrendere" >surrendere</option>
			</select><br/>
Company:<br/>
<input type="text" name="company"><br/>
Genre:<br/>
<input type="checkbox" name="genre[]" value="Action">Action</input>
<input type="checkbox" name="genre[]" value="Adult">Adult</input>
<input type="checkbox" name="genre[]" value="Adventure">Adventure</input>
<input type="checkbox" name="genre[]" value="Animation">Animation</input>
<input type="checkbox" name="genre[]" value="Comedy">Comedy</input>
<br/>

<input type="checkbox" name="genre[]" value="Crime">Crime</input>
<input type="checkbox" name="genre[]" value="Documentary">Documentary</input>
<input type="checkbox" name="genre[]" value="Drama">Drama</input>
<input type="checkbox" name="genre[]" value="Family">Family</input>
<input type="checkbox" name="genre[]" value="Fantasy">Fantasy</input>
<br/>

<input type="checkbox" name="genre[]" value="Horror">Horror</input>
<input type="checkbox" name="genre[]" value="Musical">Musical</input>
<input type="checkbox" name="genre[]" value="Mystery">Mystery</input>
<input type="checkbox" name="genre[]" value="Romance">Romance</input>
<input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi</input>
<br/>

<input type="checkbox" name="genre[]" value="Short">Short</input>
<input type="checkbox" name="genre[]" value="Thriller">Thriller</input>
<input type="checkbox" name="genre[]" value="War">War</input>
<input type="checkbox" name="genre[]" value="Western">Western</input>
<br/>

<input type="submit" name="submit" value="Add">
</form>


<?php

function test_input($data){
	$data = trim($data);
	return $data;
	}

$title = $year = $rating = $company = $genre ="";


if ($_GET["submit"]){
	$title = test_input($_GET["title"]);
	$year = $_GET["year"];
	$rating = $_GET["rating"];
	$company = test_input($_GET["company"]);
	$genre = $_GET["genre"];
}		

if ($title == ""){
	echo "Valid Title required";
	}

else if ($year == "" || $year > 2017 || $year < 1900){
	echo "Valid year required";
	}

else{
	$db = new mysqli('localhost', 'cs143', '', 'CS143');
	if($db->connect_errno > 0){
		die('Unable to connect to database [' . $db->connect_error . ']');
		}
		
	$maxpIDquery = "SELECT MAX(id) FROM MaxMovieID";

	
	if(!($rs1 = $db->query($maxpIDquery))){
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
		}
	
	$row = $rs1->fetch_assoc();
	$maxpID = $row['MAX(id)'];
	$newmaxpID = $maxpID + 1; 
	
	$title = mysql_escape_string($title);
	$company = mysql_escape_string($company);
	if ($company == ""){
		$query = "INSERT INTO Movie VALUES('$newmaxpID','$title','$year','$rating',NULL)";
		}
	else{
		$query = "INSERT INTO Movie VALUES('$newmaxpID','$title','$year','$rating','$company')";
		}

	if(!($rs2 = $db->query($query))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}
	
	for ($i=0; $i < count($genre);$i++){
		$genrequery = "INSERT INTO MovieGenre VALUES ('$newmaxpID','$genre[$i]')";
		if(!($rs3 = $db->query($genrequery))) {
			$errmsg = $db->error;
			echo "Query failed:".$errmsg."<br/>";
			exit(1);
			}
		}
	
	
		
	$update = "UPDATE MaxMovieID SET id=$newmaxpID WHERE id=$maxpID";

	if(!($rs4 = $db->query($update))) {
	$errmsg = $db->error;
	echo "Query failed:".$errmsg."<br/>";
	exit(1);
	}
	

	echo "New Movie addded with ID: ".$newmaxpID;
	
	
    $rs1->free();
    $rs2->free();
    $rs3->free();
    $rs4->free();

	$db->close();
		
}


?>

</body>
</html>