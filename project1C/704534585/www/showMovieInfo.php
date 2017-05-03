<!DOCTYPE html>
<html>

<head>

<title> Show Movie Information </title>
</head>

<body>
<h1> Contents </h1>
<a href="addActorDirector.php">Add Actor/Director </a>&nbsp;
<a href="addMovie.php">Add Movie </a>&nbsp;
<a href="addActorMovie.php">Add Movie-Actor Relations </a>&nbsp;
<a href="addDirectorMovie.php">Add Movie-Director Relations </a>&nbsp;
<a href="search.php">Search</a>
<br/>
<br/>
<h1> Show Movie Information </h1>

<?php

$mid = "";
$mid = $_GET["id"];
if ($mid == ""){
	echo "Invalid movie ID";
	echo "<br/><br/><br/><br/>";
}
else {
	$db = new mysqli('localhost', 'cs143', '', 'CS143');
	if($db->connect_errno > 0){
		die('Unable to connect to database [' . $db->connect_error . ']');
		}
	$query = "SELECT title, year, rating, company FROM Movie WHERE id=$mid";
	
	if(!($rs1 = $db->query($query))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}
	
	$row = $rs1->fetch_assoc();
	echo "<b> Title: </b>".$row["title"]."<br/>";
	echo "<b> Year: </b>".$row["year"]."<br/>";
	echo "<b> MPAA rating: <b/>".$row["rating"]."<br/>";
	if($row["company"] != ""){
		echo "<b>Company: </b> ".$row["company"]."<br/><br/>"; 
	}
	else {
		echo "<b>Company: </b> N/A";
	}

	$query2 = "SELECT D.last, D.first FROM MovieDirector MD, Director D WHERE MD.mid=$mid AND MD.did=D.id";
    
    if(!($rs2 = $db->query($query2))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}
	
	echo "<b> Director: <b/><br/>";
	while ($row2 = $rs2->fetch_assoc()){
		echo $row2["last"].",".$row2["first"]."<br/>";	
	}

	echo "<br/>";
	
	$query3 = "SELECT genre from MovieGenre WHERE mid=$mid";
    
    if(!($rs3 = $db->query($query3))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}
    echo "<b> Genre: <b/><br/>";
	while ($row3 = $rs3->fetch_assoc()){
		echo $row3["genre"]."<br/>";	
    }

    echo "<h2>Actors:</h2>";
    $query4 = "SELECT DISTINCT MA.aid, MA.role, A.last, A.first FROM MovieActor MA, Actor A WHERE MA.mid=$mid AND MA.aid=A.id";
    if(!($rs4 = $db->query($query4))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}    
	
	while ($row4 = $rs4->fetch_assoc()){
	
		echo "<a href=\"showActorInfo.php?id=".$row4["aid"]."\">".$row4["first"]." ".$row4["last"]."</a>";

		echo $link." plays ".$row4["role"]."<br/>";
    }
     
    
    echo "<h2>Average user ratings: </h2>";
    $query5 = "SELECT AVG(rating) FROM Review WHERE mid=$mid";
    if(!($rs5 = $db->query($query5))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}    
	
    $row5 = $rs5->fetch_assoc();
    if($row5["AVG(rating)"]==""){
    	echo "No rating available.<br/>";
    }
	else {
		echo $row5["AVG(rating)"]." out of 5 <br/>";
	}
    echo "<h2> Comments </h2>";
    echo "<a href=\"addComment.php?mid=".$mid."\"> Add your comment</a> <br/>";
    echo "<br/>";
    echo "<br/>";
 
    $query6 = "SELECT comment FROM Review WHERE mid=$mid ORDER BY time DESC";
    if(!($rs6 = $db->query($query6))) {
		$errmsg = $db->error;
		echo "Query failed:".$errmsg."<br/>";
		exit(1);
	}      

    $count = 1;

    while ($row6 = $rs6->fetch_assoc()){
		echo "<b>Review #".$count."</b> <br/>";
		echo "Comment: ".$row6["comment"]."<br/>";
		echo "<br/>";
		$count++;
	}
	
	$rs1->free();
	$rs2->free();
	$rs3->free();
	$rs4->free();
	$rs5->free();
	$rs6->free();
	
	$db->close();
	
}


?>

</body>
</html>