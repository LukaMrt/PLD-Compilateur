int main() {
   int i = 0;
   int j = 0;
   int k = 0;
   int count = 0;
   while (i < 2) {
       j = 0;
       while (j < 2) {
           k = 0;
           while (k < 2) {
               count = count + 1;
               k = k + 1;
           }
           j = j + 1;
       }
       i = i + 1;
   }
   return count;
}
