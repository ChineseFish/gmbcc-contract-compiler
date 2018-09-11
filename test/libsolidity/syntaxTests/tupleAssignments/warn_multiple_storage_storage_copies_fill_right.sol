contract C {
	struct S { uint a; uint b; }
	S x; S y;
	function f() public {
		(x, y, ) = (y, x, 1, 2);
	}
}
// ----
// Warning: (79-102): This assignment performs two copies to storage. Since storage copies do not first copy to a temporary location, one of them might be overwritten before the second is executed and thus may have unexpected effects. It is safer to perform the copies separately or assign to storage pointers first.
// Warning: (79-102): Different number of components on the left hand side (3) than on the right hand side (4).
