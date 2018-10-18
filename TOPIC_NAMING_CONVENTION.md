# Topic Naming Convention Guide

A. Use alphabet, number, special characters ("-", "_", ".","+")
   - Topic should start with "/" (※ Do not use trailing forward slash (/) in Topic)
   - space(" ") not accepted
   - case sensitive (※ small letters recommanded )

B. Use hierarchy field
   - Use forward slash (/) to indicate a hierarchical relationships

C. Make uniqueness for each topic
   - Only a topic name can be registered in topic name server

## Topic Discovery Features

a. Keyword mapping (including full name of topic)
   - Get all the topic lists containig queried keywords as upper field
   ex) Query : "/sevt/cnc"  => Result : {/sevt/cnc/n1/data_spindle ,/sevt/cnc/n1/data_temperature, /sevt/cnc/a2/data_humidity}

b. Wildcards discovery (TBD)
   - Can apply wildcard in one field using '*'
   ex) Query : "/sevt/cnc/*/data_spindle" => Result : {/sevt/cnc/n1/data_spindle, /sevt/cnc/a2/data_spindle, /sevt/cnc/c3/data_spindle}

 
