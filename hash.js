var num = 1023/28;
for (var i = 0; i <= 28; i++) {
 var finalNum = Math.round(num*i);
 if (i%2 === 0) {
  console.log('Note:', finalNum)
 } else {
  console.log(finalNum);
 }
} 
