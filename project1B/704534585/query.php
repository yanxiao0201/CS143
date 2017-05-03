<!DOCTYPE html>
<html>

<head>

<title> Mysql </title>
</head>

<body>
<h1> Movie Database </h1>

(Version#1 10/18/2016 Project 1B by Xiao Yan)<br/>


<form action= "query.php" method="GET">
<textarea name="comment" rows = "5" cols = "40">
</textarea>
<br/>
<input type="submit" name="submit" value="submit">
<br/>
</form>

<h2> Results From Mysql <br/> </h2>

<table border=1 cellspacing=1 cellpadding=2>
<?php
	
	if ($_GET["submit"]){
		$query = $_GET["comment"];
		$db = new mysqli('localhost', 'cs143', '', 'CS143');
		if($db->connect_errno > 0){
		die('Unable to connect to database [' . $db->connect_error . ']');
		}
		if(!($rs = $db->query($query))) {
		$errmsg = $db->error;
		echo "Query failed: $errmsg <br/>";
		exit(1);
		}
		$field_cnt = $rs->field_count;
		$finto = $rs->fetch_fields();
		
		echo "<tr>";
		
		foreach ($finto as $val){
		echo  "<th>".$val->name."</th>";
		}
		echo "</tr>";

		while($row = $rs->fetch_assoc()) {
		echo "<tr>";
		foreach ($finto as $val){
		     $data = $row[$val->name];
		     if ($data == NULL){
		         echo "<td>N/A</td>";
		     }
		     else {
		    	 echo "<td>".$data."</td>";
		    	 }
		    } 
		echo "</tr>";
		
		}
		echo 'Total rows: '.$rs->num_rows;
		$rs->free();
		$db->close();
		
		
	}
	
?>
</table>

</body>
</html>