int main() {
   int x = 5;
   int y = 0;
   int result = 0;

   while (x > 0) {
       if (x > 10) {
           result = result + 100;
       } else if (x > 5) {
           result = result + 50;
       } else if (x > 2) {
           result = result + 10;
       } else {
           result = result + 1;
       }
       x = x - 1;
   }

   y = 3;
   while (y < 8) {
       if (y == 3) {
           result = result + 5;
       } else if (y == 5) {
           result = result + 15;
       } else if (y == 7) {
           result = result + 25;
       } else {
           result = result + 2;
       }
       y = y + 1;
   }

   return result;
}
