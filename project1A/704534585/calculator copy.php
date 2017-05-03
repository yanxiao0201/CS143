<!DOCTYPE html>
<html>

<head>
<title> Calculator </title>
</head>

<body>
<h1> Calculator </h1>

(Version#1 10/2/2016 Project 1A by Xiao Yan)<br/>
Type an expression in the following box (e.g., 11.5+15*2/222).<br/>

<form action= "calculator.php" method="GET">
<input type="text" name="expression">
<input type="submit" name="submit" value="calculate">
</form>

<ul>
<li> Only numbers and +,-,* and / operators are allowed in the expression.
<li>The evaluation follows the standard operator precedence.
<li>The calculator does not support parentheses.
<li>The calculator handles invalid input "gracefully". It does not output PHP error messages.
</ul>

Here are some (but not limit to) reasonable test cases:

<ol>
<li> A basic arithmetic operation: 3+4*5=23 
<li> An expression with floating point or negative sign: -3.2+2*4-1/3 = 4.46666666667, 3*-2.1*2=-12.6
<li> Some typos inside operation (e.g. alphabetic letter): Invalid input expression 2d4+1
</ol>

<?php
	//echo "Expression" .$_GET["expression"]."<br/>";
	if ($_GET["submit"]){
		$input = $_GET["expression"];
		//deal with space
		$input = preg_replace('/\s+/','',$input);
		
		//check the exisitence of wrong symbol
		$validexp1 = preg_match('/^[\+\-\*\/\.,0-9]+$/',$input);
		
		//check start with 0 then a number
		$validexp2 = preg_match('/^[0][0-9]/',$input);
		
		//check start with operate then 0 then a number
		$validexp3 = preg_match('/[\+\-\*\/][0][0-9]/',$input);
		
		
		//if all pass valid check
		if ($validexp1 && !$validexp2 && !$validexp3){
			$divide0 = preg_match('/\/[0](\.[0]+)*$/',$input) || preg_match('/\/[0](\.[0]+)*[\+\-\*\/]/',$input);
			
			
			if ($divide0){
				$output= "Division by zero error.";
			}
			else {
				//change +- or -- to + - or - - 
				$updated_input = preg_replace('/([\+\-\*\/])([\-])/','$1 $2', $input);
			
				//evaluate and put the result to output
				eval("\$output = $updated_input;");
			
				//check if it cannot be evaluated
		   		if ($output==null){
		    	$output = "Invalid Expression";
		    	}
		    	else {
		    	$output = $updated_input." = ".$output;
		    	}
		    }
		}
	    else{
	    $output = "Invalid Expression";
	    }
	}
	
?>

<h2> Result </h2>

<?php
	echo $output;
?>

	




</body>
</html>