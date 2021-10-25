import { OriginConnectionPosition } from '@angular/cdk/overlay';
import { Component, OnInit, OnDestroy } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { EnoceanService } from '../services/enocean.service';
import { EnoceanI } from '../enocean-i';
import { EntryService } from '../services/entry.service';


// container class
export class EnOceanDisplayItem
{
  id: string;
  highlighted = false;

  constructor(public telegram: EnoceanI)
  {
    this.id = EnOceanDisplayItem.generateId(telegram);
  }

  highlight(): void
  {
    this.highlighted = true;
    setTimeout(() => this.highlighted = false, 1000);
  }

  public static generateId(telegram: EnoceanI): string
  {
    return telegram.type + telegram.senderID + telegram.payload;
  }
}

@Component({
  selector: 'app-teachin-en-ocean',
  templateUrl: './teachin-en-ocean.component.html',
  styleUrls: ['./teachin-en-ocean.component.css']
})
export class TeachinEnOceanComponent implements OnInit, OnDestroy
{
  enOceanTelegrams = new Map<string, EnOceanDisplayItem>();

  constructor(public dialog: MatDialog, private enoceanService: EnoceanService, private entryService: EntryService) { }

  ngOnInit(): void
  {
    this.startEnOceanTeachin();
  }

  startEnOceanTeachin(): void
  {
    this.enoceanService.startListening();
    this.enoceanService.getEnoceanTelegramArray().forEach((obj) => this.receiveEnOceanTelegram(obj));

    this.enoceanService.receivableTelegram.subscribe((enOceanTelegram: any) =>
    {
      console.log("New EnOcean telegram: " + enOceanTelegram);
      this.receiveEnOceanTelegram(enOceanTelegram);
    });
  }

  receiveEnOceanTelegram(telegram: EnoceanI)
  {
    let id = EnOceanDisplayItem.generateId(telegram);
    if (this.enOceanTelegrams.has(id))
    {
      console.log("Entry already exists");
      this.enOceanTelegrams.get(id)?.highlight();
    }
    else
    {
      this.enOceanTelegrams.set(id, new EnOceanDisplayItem(telegram));
    }
  }

  /* This function calles the entryService to add an enocean Signal after parsing it
   * @PARAMS:
   * enoceanid: string
   * enoceanPayload: string
  */
  addEnocean(enoceanid: string, enoceanPayload: string): void
  {
    var cleanEnPay = enoceanPayload.replace("0x", "");
    if (!enoceanPayload.endsWith("0"))
    {
      cleanEnPay = cleanEnPay.concat("0");
    }
    let enocean = enoceanid.concat(cleanEnPay + "#");
    console.table("ID = " + this.entryService.getCurrentID() + "enocean = " + enocean);
    this.entryService.addEnOcean(this.entryService.getCurrentID(), enocean);
  }

  endEnOceanTeachin(): void
  {
    this.enoceanService.stopListening();
    this.enoceanService.resetEnoceanTelegramArray();
  }

  ngOnDestroy(): void
  {
    //Called once, before the instance is destroyed.
    this.endEnOceanTeachin();
  }

}