calculator() = {
    printl("=====CALCULATOR=====");
    printl("This is a simple calculator.");
    printl("You can use +, -, *, /, and % operators.");
    printl("Type 'exit' to quit the calculator.");
    
    while (true) {
        a = input("Enter number 1: ");
        operator = input("Enter operator: ");
        c = input("Enter number 2: ");
        if (a == "exit" or c == "exit" or operator == "exit") {
            break;
        }
        a = toInt(a);
        c = toInt(c);
        if(operator == "+") {
            printl(a + "+" + c + "= " + (a + c));
        }
        elif(operator == "-") {
            printl(a + "-" + c + "= " + (a - c));
        }
        elif(operator == "*") {
            printl(a + "*" + c + "= " + (a * c));
        }
        elif(operator == "/") {
            printl(a + "/" + c + "= " + (a / c));
        }
        elif(operator == "%") {
            printl(a + "%" + c + "= " + (a % c));
        }
        else {
            printl("Invalid operator. Please use +, -, *, / or %.");
        }
    }
}

demoMain() = {
    printl("=====DEMO=====")
    printl("This is a demo of the Language interpreter.");
    printl("Test demo:")
    printl("[1] Calculator")
    while(true) {
        choice = input("Which one do you want to do: ")
        if(choice == "1") {
            calculator();
        } elif(choice == "exit") {
            printl("Exiting demo.");
            break;
        } 
        else {
            printl("Invalid choice.\n");
        }
    }
}
demoMain()