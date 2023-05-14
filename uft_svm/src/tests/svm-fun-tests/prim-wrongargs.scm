(let* ([r1 1] [r2 (+ r1)]) r1)
(let* ([r1 1] [r2 (error)]) r1)