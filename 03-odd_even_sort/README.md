Implement in parallel the odd even sort. Use as input a random generated integer vector. The odd even sort algorithm looks 
like the following (this is sequential; n is the length of the vector, v is the vector to sort, swap is a procedure that 
exchanges the two positions in the vector): 

while(not(sorted)) {
  sorted = true; 
  for(i=1; i<n-1; i+=2)
    if(v[i] > v[i+1]) { swap(v[i], v[i+1]); sorted = false; } 
  for(i = 0; i<n-1; i+=2) 
    if(v[i] > v[i+1]) { swap(v[i], v[i+1]); sorted = false; } 
}

with the intent to discuss scalability of the code.
