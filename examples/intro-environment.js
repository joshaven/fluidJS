var env   = system.ENV;
var print = system.shell.print;
print('\nsystem.ENV:' + JSON.stringify(env) );
print('\nsystem.ENV.args:: ' + env.args);
print('\nsystem.ENV.argv:: ' + env.argv);
print('\nsystem.ENV.argc:: ' + env.argc);
