#Use hash for comments
import "@package/packageName" # This will auto download package from our package manager (like npmjs.com), cache it, and import it
import force "http://example.com/packageName" # This download package from the given url, import it, but never cache it bcuz of "force" keyword so it download everytime script run

class Hello {
    hello = 1
    getHello() = {
        return hello
    }
    #changing hi will also change hello since hi is a reference to hello
    setHi(value) = {
        hi = value
    }
}

#This define a function with name functionName and parameters x and y
#The syntax is refered from math's function notation (f : x, y -> x + y)
#identify whether the functionName token is a function by the second token being a colon
functionName(x, y) = {
    return x + y #this returns the sum of x and y
}

#This will call the function we just created
functionName(5, 10) #This will return 15
log(functionName(5, 10)) #This will log the result to the console
#identify whether the variableName token is a variable by the second token being a equal sign
variableName = 42 #This will assign the value 42 to variableName with type "number"

if (variableName > 40) {
    log(`Variable is greater than 40, value: {variableName}`) #This will run if the condition is true
    # `` mean formatted string
} else {
    log("Variable is less or equal 40, value: {variableName}") #This will run if the condition is false
    # This is normal string
}
#{} is used to define a block of code, like in dart

ref referenceVariable = variableName #This will create a reference to variableName
referenceVariable = 100 #This will change the value of variableName to 100
variableName = 50 #This will change the value of referenceName to 50
#Changing value of each one will always change the other one

del referenceVariable #This will delete the variable 'referenceVariable'
del variableName #This will delete the variable 'variableName'
# by deleting a variable, it will be removed from memory and cannot be used anymore
# when u define a variable in function, it will also be deleted when the function ends (out of scope)

loop i = 0 until i >= 10 { #use until for readability, until is only used in loop keyword
    log(i) #This will log the value of i from 0 to 10
}

i = 10 #This will define a new variable i since the i in the loop is out of scope (which mean auto delete)

while (i > 0) {
    log(i) #This will log the value of i from 0 to 10
    i -= 1.1 #This will increment i by 1
}

# we can store using map<string, variableClass> to store variables with string keys
# there should be map in every scope. Like in function scope there should be another map so outside can't access variable inside
# and memory will be automatically freed once out of scope (function ends)