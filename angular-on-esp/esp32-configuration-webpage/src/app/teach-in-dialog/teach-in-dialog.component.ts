import { Component, Input, OnDestroy, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { EnoceanI } from '../enocean-i';
import { EnoceanService } from '../services/enocean.service';
import { EntryService } from '../services/entry.service';
import { IRServiceService } from '../services/irservice.service';
import { TeachinEnOceanComponent } from '../teachin-en-ocean/teachin-en-ocean.component';


@Component({
  selector: 'app-teach-in-dialog',
  templateUrl: './teach-in-dialog.component.html',
  styleUrls: ['./teach-in-dialog.component.scss']
})
export class TeachInDialogComponent implements OnInit, OnDestroy
{

  toggle = false;

  inputString = "";
  stage = 0;

  irSignal = "";

  // create new array one array to recive and one array to hold
  irSignals: string[] = [];
  noDuplicationIr: string[] = [];


  constructor(public dialog: MatDialog, private irService: IRServiceService, private entryService: EntryService) { }

  ngOnInit(): void
  {
  }

  // highlight(): void
  // {
  //   this.toggle = true;
  //   setTimeout(() => this.toggle = false, 1000);
  // }

  /*WIP function not working*/
  // reciveIr(irSignal: string): void
  // {
  //   console.warn(this.noDuplicationIr.length);
  //   let found = false;
  //   // if (this.noDuplicationIr.length < 1)
  //   // {
  //   //   this.noDuplicationIr.push(irSignal);
  //   // } else
  //   // {
  //   this.noDuplicationIr.forEach(element =>
  //   {
  //     if (element.length == irSignal.length)
  //     {
  //       // compare string 90% same then true;
  //       let elementx = element.split(',');
  //       let irSignalx = irSignal.split(',');
  //       let n: number = 0;
  //       for (let i = 0; i < elementx.length; i++)
  //       {
  //         if (elementx[i] === irSignalx[i])
  //         {
  //           console.log("elementx[" + i + "] = " + elementx[i])
  //           n++
  //         }
  //       }
  //       let similarity = n / irSignalx.length;
  //       console.log("Similarity = n / irSignalx.lenght " + n + " / " + elementx.length + " = " + similarity);
  //       if (similarity > 0.8)
  //       {
  //         console.log("Signal already recived");
  //         this.highlight();
  //         found = true;
  //       }
  //     }
  //   });
  //   if (!found)
  //   {
  //     console.log("adding signal"); // FOR DEBUG
  //     this.noDuplicationIr.push(irSignal);
  //   }
  // }

  setChosenIR(ir: string)
  {
    this.irSignal = ir;
  }

  sendTestIr(irsignal: string)
  {
    this.entryService.testIR(irsignal);
  }

  getChosenIR(): string
  {
    return this.irSignal;
  }

  isNextAvailable(): boolean
  {
    let result = false;
    if (this.stage == 0 && this.inputString && this.inputString.length > 0)
      result = true;

    if (this.stage == 1 && this.irSignal && this.irSignal.length > 0)
      result = true;

    return result;
  }

  next(): void
  {
    this.stage++;
    console.log(this.inputString);

    if (this.stage == 1)
    {
      this.irService.startListening();
      this.irSignals = this.irService.getIrSignals();
      this.irService.receivableSignal.subscribe((irSignal: string) =>
      {
        console.log("IR signal: " + irSignal); // For Debug
        this.irSignals.push(irSignal); // check if can delete
        // this.reciveIr(irSignal);
      });
    } else
    {
      this.irService.stopListening();
      this.irService.resetIrSignals();
    }
    if (this.stage == 2)
    {
      this.entryService.addEntry(this.inputString, this.getChosenIR());
      this.dialog.closeAll(); // Closes all of the currently-open dialogs.
      this.dialog.open(TeachinEnOceanComponent); // Open TeachInEnOceanDialog
    }
  }

  ngOnDestroy(): void
  {
    //Called once, before the instance is destroyed.
    this.irService.stopListening();
    this.irService.resetIrSignals();
  }
}
