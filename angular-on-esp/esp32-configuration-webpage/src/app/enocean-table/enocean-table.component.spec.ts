import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EnoceanTableComponent } from './enocean-table.component';

describe('EnoceanTableComponent', () => {
  let component: EnoceanTableComponent;
  let fixture: ComponentFixture<EnoceanTableComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ EnoceanTableComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(EnoceanTableComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
