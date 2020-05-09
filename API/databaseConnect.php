 <?php
  $servername = "localhost";
  $username = "";
  $password = "";
  $dbname = "";
  
  // Create connection
  $conn = new mysqli($servername, $username, $password, $dbname);
  
  // Check connection
  if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
  }
  
  $mode = $_GET['Mode'];
  $cooking = 0;
  
    // -------------------------------- //
    //       Display records            //
    // -------------------------------- //
    
  if ( $_SERVER['REQUEST_METHOD'] == "GET" ) {
     
    if ( $mode == 1) {
        $name = $_GET['Name'];
        $sql = "SELECT * FROM Profiles WHERE Name='".$_GET['Name']."' ";
    }
    elseif ( $mode == 2 || $mode == 3) {
         $sql = "SELECT * FROM Queue";
    }

    $select_results = $conn->query($sql);
    
    if ( $select_results->num_rows > 0 ) { // Verify at least 1 result is desplayed
     
        // output data of each row
        while ( $row = $select_results->fetch_assoc() ) {
          // retrieve user data
          if ( $mode == 1) {
            echo $row["Name"] . "|" . $row["History"] . "|" .  $row["Favorites"] .  "<br>";
          }
          
          // retrieve cooking data
          elseif ( $mode == 2) {
               
               // cooking in progress? finished? -> yes, delete : no, break continue
               if ($row["StartTime"] != null  || $cooking == 1){   
                    //check if cooking is complete (based on CookingTime in DB)
                    if (time() >= strtotime( $row["StartTime"] ) + $row["CookingTime"]  * 60 ) {
                      //delete finished meal from queue
                      $sql_UpdateTime = "DELETE FROM Queue WHERE StartTime='".$row["StartTime"]."' , Name='".$row["Name"]."' ";
                      $conn->query($sql_UpdateTime);
                      $cooking = 0;
                      break;
                    }
               }
               
              elseif ( $row["StartTime"] == null){
                 // print slot number for Pi init
                 echo $row["Slot"] . " ";
                 //update slot time stamp
                 $sql_UpdateTime = "UPDATE Queue SET StartTime='".date("h:i")."' WHERE Slot='".$row["Slot"]."' ";
                 $conn->query($sql_UpdateTime);
                 $cooking = 1;
                 break;             
               }       
          }
          
          // retrieve queue data
          elseif ($mode == 3){
               // just print queue list with user
               echo "Slot: " . $row["Slot"] . " | start time: " . $row["StartTime"] . "| Cooking Time:" .  $row["CookingTime"] .  " | By: " .  $row["Name"] . "<br>";
           }
        }
    } else {
        echo "  empty  ___ ";
    }
  }
      // -------------------------------- //
      //          Update record           //
      // -------------------------------- //
  elseif ( $_SERVER['REQUEST_METHOD'] == "POST" ) {
      if ( $mode == 1) {
          echo "test";
          $sql = "UPDATE Profiles SET History=concat(History,','".$_GET['orderMeal']."' ') WHERE Name='".$_GET['Name']."'  ";
          $conn->query($sql);
      }
      
     // -------------------------------- //
     //          New Order               //
     // -------------------------------- //
      elseif ( $mode == 2 ) {
          $sql = "INSERT INTO `Queue` (`Slot`, `CookingTime`, `Name`) VALUES ('".$_GET['slot']."' , 10, '".$_GET['Name']."') ";
          $conn->query($sql);          
      }
     
      // -------------------------------- //
      //          New User                //
      // -------------------------------- //
      elseif ( $mode == 3 ) {
          $sql_newUser = "INSERT INTO `Profiles` (`Name`, `UniqueID`) VALUES ('".$_GET['name']."' , '".$_GET['uniqueID']."') ";
          $conn->query($sql_newUser);
      } 
      else {
          echo "incorrect mode option";
      }
 
  } else {
    http_response_code(405);
  }
  
  http_response_code(200);
  $conn->close();
?> 