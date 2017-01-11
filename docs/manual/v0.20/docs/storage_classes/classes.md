## Classes

### Variables

As in typical imperative languages, a variable in Céu holds a value of a
[declared](#TODO) [type](#TODO) that may vary during program execution.
The value of a variable can be read in [expressions](#TODO) or written in
[assignments](#TODO).
The current value of a variable is preserved until the next assignment, during
its whole lifetime.

<!--
TODO: exceptions for scope/lifetime
- pointers have "instant" lifetime, like fleeting events, scope is unbound
- intermediate values die after "watching", scope is unbound
-->

*Note: since blocks can contain parallel compositions, variables can be read
       and written in trails in parallel.*

Example:

```ceu
var int v = _;  // empty initializaton
par/and do
    v = 1;      // write access
with
    v = 2;      // write access
end
escape v;       // read access (yields 2)
```

### Vectors

In Céu, a vector is a dynamic and contiguous collection of elements of the same
type.
A vector [declaration](#TODO) specifies its type and the maximum number of
elements (possibly unlimited).
The current size of a vector is dynamic and can be accessed through the
[operator `$`](#TODO).
Individual elements of a vector can be accessed through a
[numeric index](#TODO) starting from `0`.

Example:

```ceu
vector[9] byte buf = [1,2,3];   // write access
buf[$buf+1] = 4;                // write access
escape buf[1];                  // read access (yields 2)
```

### Events

Events are the most fundamental concept of Céu, accounting for its reactive 
nature.
Programs manipulate events through the `await` and `emit` [statements](#TODO).
An `await` halts the running trail until that event occurs.
An event occurrence is broadcast to the whole program and awakes trails
awaiting that event to resume execution.

As described in the [Introduction](#TODO), Céu supports external and internal events
with different behaviors.

Unlike all other storage classes, the value of an event is ephemeral and does
not persist after a reaction terminates.
For this reason, an event identifier is not a variable: values can only
be communicated through `emit` and `await` statements.
A [declaration](#TODO) includes the type of value the occurring event carries.

*Note: <tt>void</tt> is a valid type for signal-only internal events.*

Example:

```ceu
input  void I;           // "I" is an external input event that carries no valuess
output int  O;           // "O" is an external output event that values of type "int"
event  int  e;           // "e" is an internal event that carries values of type "int"
par/and do
    await I;             // awakes when "I" occurs
    emit e(10);          // broadcasts "e" passing 10, awakes the "await" below
with
    var int v = await e; // awaits "e" assigning the received value to "v"
    emit O(v);           // emits "O" back to the environment passing "v"
end
```

### External Events

External events are used as interfaces between programs and devices from the 
real world:

* *input* events represent input devices, such as sensors, switches, etc.
* *output* events represent output devices, such as LEDs, motors, etc.

The availability of external events depends on the platform in use.
Therefore, external declarations only make pre-existing events visible to a 
program.
Refer to [Environment](#TODO) for information about interfacing with 
external events at the platform level.

#### External Input Events

As a reactive language, programs in Céu have input events as entry points in
the code through [await statements](#TODO).
Input events represent the notion of [logical time](#TODO) in Céu.

Only the [environment](#TODO) can emit inputs to the application.
Programs can only `await` input events.

#### External Output Events

Output events communicate values from the program back to the
[environment](#TODO).

Programs can only `emit` output events.

### Internal Events

Internal events serve as signalling and communication mechanisms between
trails in a program.

Programs can `emit` and `await` internal events.

### Pools

A pool is a dynamic container to hold running [code abstractions](#TODO).
A pool [declaration](#TODO) specifies the type of the abstraction and maximum
number of concurrent instances (possibly unlimited).
Individual elements of a pool can only be accessed through [iterators](#TODO).
New elements are created with [`spawn`](#TODO) and are removed automatically
when and only the code execution terminates.

Example:

```ceu
code/await Anim (void) => void do       // defines a code abstraction
    ...
end
pool[] Anim ms;                         // declares an unlimited container for "Move" instances
loop i in [0->10[ do
    spawn Anim() in ms;                 // creates 10 instances of "Anim" into "ms"
end
```

When a pool declaration goes out of scope, all running code abstractions are
automatically aborted.

`TODO: data`