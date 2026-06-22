int main() {
   int i = 0;
   int sum = 0;

   while (i < 10) {
       if (i == 3) {
           return 30;
       } else if (i == 5) {
           return 50;
       } else if (i == 7) {
           return 70;
       } else {
           sum = sum + i;
       }
       i = i + 1;
   }

   if (sum > 0) {
       return sum;
   } else {
       return 99;
   }
}
