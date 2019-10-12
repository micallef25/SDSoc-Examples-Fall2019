# SDSoc-Examples-Fall2019
Streaming Examples to help with the final project


Putting it all together!

In Homework 6 you shold have learned about DMA transfers using async, and instantiating multiple resources at a top level

So now you can do something like this!

![](img/2resources_top.png)

In homework 7 you should have been exposed to streaming between two applications inside the FPGA.

But what if we want something like this?

![](img/2resources_Streaming.png)

We want to identify our bottle necks at the source, thus utilizing less resources. We do not necessarily need a whole new pipeline. That could be costly on hardware!
We just need another App2!

This code delves deep into how to do this. Please go through and read the comments and think of any questions that were asked. 

Come to OH or post on piazza with any questions that you may have.