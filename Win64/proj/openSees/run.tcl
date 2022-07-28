set t [getTime]
set ok 0

while {$t <= $endTime && $ok == 0} {
    set ok [analyze 1 $dt]
    set t [getTime]
}