m4_divert(-1)

# _my_for(var, from, to, act)
m4_define([____for], [____for_impl([$1], $2, $3, [$4])])

m4_define([____for_impl],[m4_ifelse(m4_eval($2 < $3), [1], [m4_pushdef([$1], [$2])$4____for_impl([$1], m4_incr($2),$3,[$4])m4_popdef([$1])])])


m4_divert[]m4_dnl
