import { OriginConnectionPosition } from '@angular/cdk/overlay';
import { Component, OnInit, OnDestroy } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { EnoceanService } from '../services/enocean.service';
import { EnoceanI } from '../enocean-i';
import { EntryService } from '../services/entry.service';

@Component({
  selector: 'app-teachin-en-ocean',
  templateUrl: './teachin-en-ocean.component.html',
  styleUrls: ['./teachin-en-ocean.component.css']
})
export class TeachinEnOceanComponent implements OnInit, OnDestroy {

  enOceanTelegrams!: EnoceanI[];

  constructor(public dialog: MatDialog, private enoceanService: EnoceanService, private entryService: EntryService) { }

  ngOnInit(): void {
    this.startEnOceanTeachin();
  }

  startEnOceanTeachin(): void {
    this.enoceanService.startListening();
    this.enOceanTelegrams = this.enoceanService.getEnoceanTelegramArray();
    this.enoceanService.receivableTelegram.subscribe((enOceanTelegram: any) => {
      console.log("New EnOcean telegram: " + enOceanTelegram);
      this.enOceanTelegrams.push(enOceanTelegram);
    });
  }

  /* This function calles the entryService to add an enocean Signal after parsing it
   * @PARAMS:
   * enoceanid: string
   * enoceanPayload: string
  */
  addEnocean(enoceanid: string, enoceanPayload: string): void {
    var cleanEnPay = enoceanPayload.replace("0x", "");
    let enocean = enoceanid.concat(cleanEnPay + "#");
    console.table("ID = " + this.entryService.getCurrentID() + "enocean = " + enocean);
    this.entryService.addEnOcean(this.entryService.getCurrentID(), enocean);
  }

  endEnOceanTeachin(): void {
    this.enoceanService.stopListening();
    this.enoceanService.resetEnoceanTelegramArray();
  }

  ngOnDestroy(): void {
    //Called once, before the instance is destroyed.
    this.endEnOceanTeachin();
  }

}