## Tarnish Language Type Checking System
<div style="text-align: right">CS440</div>
<div style="text-align: right">Professor: Suman Saha</div>
<div style="text-align: right">Tony Fleck and Chunhao Li</div>

### How to build
The type checking system can be built using the command `make`.

It will automatically output to tarnish, and can be run with `./tarnish`.

### Test cases
For the convenient of testing, we provide serveral basic programs. All of the Tarnish program should end with `.tarn`.

To run the program with Tarnish type checking System, simply use:

```
> ./tarnish fileName.tarn
```

To run the programs we provide, use these commands:

```
> ./tarnish assignments.tarn
> ./tarnish if.tarn
> ./tarnish loop.tarn
```

### Tarnish Interpreter
If your run `./tarnish` without given a tarn file, you will be given a Tarnish interpreter, you can try Tarnish Programming Language without creating a file.

Here are some examples to try in the Tarnish interpreter:

**basic assignment and if condition:**

```
put 10 in int a;
put 20 in int b;

print a; //This should print 10
print b; //This should print 20

if(b > a){
	put b in a;
}

print a; //This should print 20

put 25 in a; //reassignment

print a; //This should print 25
```

**operations between numbers:**

```
put 10 in int a;
put 20 in int b;

print a + b; //This should print 30
print a - b; //This should print -10
print a * b; //This should print 200
print b / a; //This should print 2

```

**while loop**

```
put 0 in int a;

while (a < 10) {
	print a;
	put a + 1 in a;
}

print a;
/* This will print 0-10 */
```

### Type checking system
You can only do operations between same type variables, for example, these are legal in the Tarnish type checking system:

```
put 1 in int a;
put 2 in int b;
put "Hello" in string c;
put "there"in string d;

print a + b; //print 3
print c + d; //print "Hello there"
```

However, these operations are illegal:

```
put 5 in int a;
put "Hola" in string b;
put true in bool c;
put false in bool d;

/* Below Are The Illegal Operations */
print a + b;
print c + d;
```

***
#### *Our demo reports are in a separate file.