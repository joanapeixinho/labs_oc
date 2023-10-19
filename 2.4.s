                .data
A:              .word   1, 3, 1, 6, 4
                .word   2, 4, 3, 9, 5
mult:   .word   0

        .code
        daddi   $1, $0, A      ; *A[0]
        daddi   $5, $0, 1      ; $5 = 1 ;; i    
        daddi   $6, $0, 10     ; $6 = N ;; N = 10
        lw      $9, 0($1)      ; $9 = A[0]  ;; mult

        lw      $12, 8($1)     ; $12 = A[1]
        lw      $13, 16($1)     ; $12 = A[2]
        lw      $14, 24($1)     ; $12 = A[3]

loop: 
        dmul    $15, $12, $9   ; $15 = $12*$9 ;; $12 = A[i]*mult
        daddi   $5, $5, 1      ; i++
        daddi   $1, $1, 24      ;
        lw      $12, 8($1)     ; $12 = A[i+1]
        dadd    $9, $9, $15    ; $9 = $9 + $15  ;; mult = mult + A[i]*mult

        dmul    $16, $13, $9   ; $16 = $13*$9 ;; $12 = A[i]*mult
        daddi   $5, $5, 1      ; i++
        lw      $13, 16($1)     ; $13 = A[i+2]
        dadd    $9, $9, $16    ; $9 = $9 + $16  ;; mult = mult + A[i]*mult

        dmul    $17, $14, $9   ; $17 = $14*$9 ;; $12 = A[i]*mult
        daddi   $5, $5, 1      ; i++
        lw      $14, 24($1)     ; $14 = A[i+3]
        dadd    $9, $9, $17    ; $9 = $9 + $17  ;; mult = mult + A[i]*mult
        
        bne     $6, $5, loop   ; Exit loop if i == N
        
        sw      $9, mult($0)   ; Store result
        halt

;; Expected result: mult = f6180 (hex), 1008000 (dec)