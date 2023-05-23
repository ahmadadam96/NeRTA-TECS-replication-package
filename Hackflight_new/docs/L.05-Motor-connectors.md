Now it's time to solder on the connectors you'll need to power the board, run the motors,
and get the signal from the R/C receiver.  For a tiny board like this, we suggest using
[.015" solder](https://www.amazon.com/gp/product/B005T8NL22/ref=oh_aui_search_detailpage?ie=UTF8&psc=1)
and a [pencil-tip](https://www.amazon.com/gp/product/B0002BSP4K/ref=oh_aui_search_detailpage?ie=UTF8&psc=1)
on your iron, to avoid the blobbing you can get with  thicker solder or a screwdriver tip.

If you prefer to solder your motor wires directly to the flight controller, you can skip this step; 
however, we've found that using tiny "Picoblade"
[Molex connectors](https://www.digikey.com/product-detail/en/molex-llc/0530470210/WM1731-ND/242853)
makes it easier to swap out motors.  (As discussed 
[here](https://www.rcgroups.com/forums/showthread.php?1493712-JST-connector-confusion-the-real-story), 
you'll also see these connectors referred to erroneously as JST.) If you're
going to use these connectors, you'll want to attach them now, because it will
be more difficult to solder them to the board once you've attached the battery
connector and the receiver.

Here's a photo of the board with all four motor connectors attached.  (The
different coloring of the connectors on opposite sides is meaningless; those
are just the connectors that I pulled out of the bag.)  Note that for the
proper polarity, <b>the holes on the connectors face in</b>:

<img src="images/allfourmolex.png">

<p>

Getting the connectors to stick to the board while you're soldering them can be frustrating, but
I've found that putting a tiny amount of cyanoacrylate adhesive ("Crazy Glue")
on the bottom edge of the connector will keep them in place long enough for you
to solder them.  To avoid smearing the adhesive on the electrical contacts, you can squeeze a
drop of it onto a piece of paper, then use a thumb tack or pin to daub a tiny bit of adhesive onto
the connector bottom (hat tip to [Steve
Goryl](https://www.youtube.com/watch?v=HUGfP-KM3SQ) for suggesting this technique).

<img src="images/crazyglue.png">

<p>

To hold the board in place while you solder the connectors, you can use a 
[Helping Hand](https://www.amazon.com/SE-MZ101B-Helping-Magnifying-Glass/dp/B000RB38X8/ref=sr_1_3?ie=UTF8&qid=1504559137&sr=8-3&keywords=helping+hands), but make sure to cushion the clip first with some 
[heat-shrink tubing](https://www.amazon.com/gp/product/B00Q7V49RQ/ref=oh_aui_search_detailpage?ie=UTF8&psc=1),
and make sure it isn't gripping the delicate MOSFETs on the bottom side of the board:

<img src="images/helping-hand.png">

<p>

The image above shows one of the four connectors soldered onto the LadybugFC.  Note the clean separation
between the two solder joints on the connector.  Even if you're careful, it's possible accidentally to solder
a "bridge" between the two connector pins, causing a dangerous short-circuit:

<img src="images/solder-bridge.png">

<p>

If this happens, you can usually remove the bridge by gently swiping the soldering tip between the bridged
connectors.  For a really big, blobby bridge you may be better off using a 
[solder sucker](https://www.amazon.com/gp/product/B003FHYL7I/ref=oh_aui_search_detailpage?ie=UTF8&psc=1) to remove all the solder and trying again.

<hr>

<b>Next</b>: [Receiver connection](https://github.com/simondlevy/Hackflight/wiki/L.06-Receiver-connection)
