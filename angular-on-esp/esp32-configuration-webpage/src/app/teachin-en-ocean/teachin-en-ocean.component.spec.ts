import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TeachinEnOceanComponent } from './teachin-en-ocean.component';

describe('TeachinEnOceanComponent', () => {
  let component: TeachinEnOceanComponent;
  let fixture: ComponentFixture<TeachinEnOceanComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ TeachinEnOceanComponent ]
    })
    .compileComponents();
  });

  beforeEach(() => {
    fixture = TestBed.createComponent(TeachinEnOceanComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
