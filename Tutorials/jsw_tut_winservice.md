# Windows Services Simplified

  

I write a lot of Windows services...a *lot* of them. While the service
template project in Visual Studio is certainly functional, my experience
is that it tends to be unnecessarily complicated for most purposes. This
is understandable as the template is designed to be everything to
everyone. However, most of the time I don't want a fully featured
service spread across half a dozen files. This complication promotes
missing necessary changes and affects maintainability.

So what is a developer to do? Write a simplified template that does
*only* what is needed\! And that is exactly what I did. Barring an
installer, a basic service project would contain only two files:

  - **Program.cs**: Setup and startup.
  - **Service.cs**: The actual service.

I usually separate the service logic from application-specific
processing logic using a worker class that the service class calls into,
but that's a bit out of scope for this article. Program.cs sets up
logging, and custom application settings, and runs the service normally.
Service.cs hooks into the Windows SCM interface and typically does as
little as possible directly. But enough jabbering, let's look at the
bare bones template that I start with:

    /// <summary>
    /// Service controller for the process.
    /// </summary>
    public class Service : ServiceBase
    {
        /// <summary>
        /// Creates and initializes a new service.
        /// </summary>
        public Service()
        {
            CanShutdown = true;
            CanStop = true;
            CanHandlePowerEvent = false;
            CanHandleSessionChangeEvent = false;
            CanPauseAndContinue = false;
        }
    
        #region Public Interface
        /// <summary>
        /// Starts the service.
        /// </summary>
        public void StartService(string[] args) { OnStart(args); }
    
        /// <summary>
        /// Stops the service.
        /// </summary>
        public void StopService() { OnStop(); }
    
        /// <summary>
        /// Shuts down the service.
        /// </summary>
        public void ShutdownService() { OnShutdown(); }
        #endregion
    
        #region Service Control Interface
        protected override void OnStart(string[] args) { SwitchOn(); }
        protected override void OnStop() { SwitchOff(); }
        protected override void OnShutdown() { SwitchOff(); }
        #endregion
    
        #region Miscellaneous Helpers
        /// <summary>
        /// Initializes and starts the service worker.
        /// </summary>
        private void SwitchOn()
        {
            try
            {
                // Run the service worker
            }
            catch (Exception ex)
            {
                ExitCode = 1;
                // Log errors here
                throw;
            }
        }
    
        /// <summary>
        /// Stops and cleans up the service worker.
        /// </summary>
        private void SwitchOff()
        {
            // Stop and dispose the service worker
        }
        #endregion
    }

This is all you need for a basic functional service, and it highlights
just how over-engineered the Visual Studio service template is. We do
three things:

  - Inherit from the `ServiceBase` class found in
    `System.ServiceProcess`.
  - Initialize supported feature flags in the constructor. In this case,
    the service can only start, stop, and shut down. For this bare bones
    template, power events, session changes, and pause/continue are not
    directly supported. However, setting these properties to `true` and
    then adding code to handle the events is straightforward. These
    flags are set according to my typical use cases, but you can
    certainly modify the template to better suit your needs.
  - Override the service control interface. These are the underlying
    handlers which respond to service control events. In this case, the
    same events enabled by the feature flags are overridden.

That's it for Service.cs\! Program.cs is equally simple, and could be
even more simple without some of my preferred options. Obviously we need
a `Main` function, and somewhere therein call `ServiceBase.Run` with an
instance of the service class. I like to include logging here, but we'll
ignore that for the purposes of this article. The key option I've found
to be invaluable is supporting running the service executable as a
console application to separate it completely from the Windows SCM. I
call this 'Debug Mode', and it's the primary reason why the public
methods are provided in the service class. Let's look at the bare bones
Program.cs:

    public class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        public static void Main(string[] args)
        {
            if (Environment.UserInteractive)
            {
                StartConsole(args);
            }
            else
            {
                StartService(args);
            }
        }
    
        /// <summary>
        /// Runs the process as an installed service.
        /// </summary>
        private static void StartService(string[] args)
        {
            ServiceBase.Run(new ServiceBase[] { new Service() });
        }
    
        /// <summary>
        /// Runs the process in a console window.
        /// </summary>
        private static void StartConsole(string[] args)
        {
            try
            {
                Console.Title = Assembly.GetExecutingAssembly().GetName().Name + " [Debug Mode]";
    
                // Run the service worker
    
                Console.Read();
    
                // Stop and dispose the service worker
            }
            catch (Exception ex)
            {
                // Log the error
                Console.Read();
            }
        }
    }

There's nothing to it. If the application is run interactively, we call
`StartConsole` to run it as a console application. Otherwise,
`ServiceBase.Run` is executed on a new instance of the service class.
This design requires the assembly to be built as a console application.
This doesn't affect anything when run as a service, it only supports
debug mode.

Note that this is where having a separate worker class is beneficial as
you can completely isolate the service logic from the SCM boilerplate.
It's very useful for development testing and production troubleshooting.

-----

© 2018 - Eternally Confuzzled
