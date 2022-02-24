# escape=`

#	https://docs.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library?redirectedfrom=MSDN&view=msvc-170
#	in the c++ project, go to properties
#	C/C++ -> Code Generation -> Runtime Library to MTd


#	windows/servercore:lts2019 comes with .NET 4.7, so you can use it for .NET Console apps, but not ASP.NET or .NET Core apps;
#	dotnet/framework/runtime:4.8 has the last supported version of .NET Framework which you can use to run containerized console apps;
#	dotnet/framework/sdk:4.8 has MSBuild, NuGet and all the targeting packs installed, so you should be able to build pretty much any .NET Framework app - you'll use this in the builder stage only;
#	dotnet/framework/aspnet:4.8 has ASP.NET 4.8 installed and configured with IIS, so you can use it for any web apps - WebForms, MVC, Web API etc.


#	Nanoserver liefert keine Powershell somit kann keine CMD ausgegeben werden, das Programm starten funktioniert aber
#	Daher Windows servercore benutzen
#FROM mcr.microsoft.com/dotnet/framework/runtime:4.8-windowsservercore-ltsc2019
#FROM mcr.microsoft.com/dotnet/framework/sdk:4.8-windowsservercore-ltsc2019
#FROM mcr.microsoft.com/dotnet/framework/sdk:4.8-windows-ltsc2019
#	kleiner als das dotnet framework (solange das reicht, das benutzen)
#	sdk ist zum builden da, da wir aber nur deployen wollen, reicht der server
FROM mcr.microsoft.com/windows/servercore:10.0.19041.1415
#docker pull mcr.microsoft.com/windows/server/insider
#FROM mcr.microsoft.com/windows:10.0.19041.1415
#FROM mcr.microsoft.com/windows:10.0.19041.1415

# Reset the shell.
SHELL ["cmd", "/S", "/C"]

#copy all files
ADD . C:\Temp\

#Start application
#CMD "C:\\Temp\\BinanceTradingBot.exe"
ENTRYPOINT ["cmd.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]