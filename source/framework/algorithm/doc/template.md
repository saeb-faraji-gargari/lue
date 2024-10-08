(source-framework-algorithm-templates)=

# Templates


## Algorithm template parameters

Most algorithms are function templates, requiring information provided at compile-time. Decisions must be made
about the order of these. It is nice if no template arguments have to be explicitly provided at the call side
because they can be deduced from the arguments passed in.

In case policies are passed in, the element types of any inputs and outputs can be deduced from the `Policies`
type. See recently added algorithms for how to do this. Older algorithms often have template parameters for
policies, and for the element types of the inputs and outputs, while newer algorithms deduce these element
types from the policies arguments. See `policy::InputElemenT<Policies, argument_idx>` and
`policy::OutputElementT<Policies, result_idx>`.

If no policies are passed in, as is the case of algorithms defined in the `default_policies` and
`value_policies` namespaces, any output element types that cannot be deduced from the types of the arguments
should be moved to the front of the template parameter list. Otherwise, more types than necessary have to be
specified when calling the algorithm.


## Explicit algorithm template instantiation

Templates are instantiated at compile time. Instantiating the same template at multiple places in the code is
often fine, but in the case of LUE algorithm this greatly increases the build times and memory requirements.
Therefore, most of the algorithm are explicitly instantiated for the common set of policies and array ranks.
It is still possible to instantiate the algorithms for other policies and ranks, of course.

Code for the explicit instantiations is generated by the `generate_template_instantiation` macro in
`source/framework/algorithm/CMakeLists.txt`. A source file is generated for each individual instantiation,
decreasing the amount of memory required for compiling instantiations, at the expense of increasing the number
of source files to compile. Object files containing instantiated templates are linked into a set of libraries.
Depending on which algorithms used by a target (e.g.: a unit test), the target must be linked to one or more
of these libraries.
