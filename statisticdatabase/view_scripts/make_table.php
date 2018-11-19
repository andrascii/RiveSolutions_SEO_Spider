<?php

function makeTable($result)
{
    $sqlQueryResult = mysqli_fetch_assoc($result);
    
    echo "<table align='center'><tr class='tableHeader'>";
    
    foreach($sqlQueryResult as $key => $value)
    {
        echo "<td align='center'><h3>" . $key . "</h3></td>";
    }
    
    echo "</tr>";
    
    do
    {
        echo "<tr>";
        
        foreach($sqlQueryResult as $key => $value)
        {
            echo "<td>" . $value . "</td>";
        }
        
        echo "</tr>";
        
    } while($sqlQueryResult = mysqli_fetch_assoc($result));
    
    echo "</table>";
}

?>