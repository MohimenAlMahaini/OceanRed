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
