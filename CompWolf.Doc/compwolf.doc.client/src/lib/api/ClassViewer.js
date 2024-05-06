import { Fragment } from "react";
import { Declaration, Reference, SimpleReference } from "../CodeComponents";
import FormattedText from "../FormattedText";
import BaseEntityViewer from "./BaseEntityViewer";
import betterEncodeURIComponent from "../betterEncodeURIComponent";

export default function ClassViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	function MemberReference({ name }) {
		return (
			<Reference path={`/api/
			${data.project}/
			${data.header}/
			${(data.owners ?? []).map(x => `${betterEncodeURIComponent(x)}/`).join()}
			${betterEncodeURIComponent(data.name)}/
			${betterEncodeURIComponent(name)}`}>
				{name}
			</Reference>
		)
	}

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<small hidden={is_empty(data.baseClasses)}>
					Inherits from: {
						is_empty(data.baseClasses) ? "nothing." : <ul>
							{data.baseClasses.map((x, i) => {
								return (
									<li key={i}>
										<SimpleReference name={x} />
									</li>
								)
							})}
						</ul>
					}
				</small>
				<big id="Declaration">
					<Declaration>{data.declaration}</Declaration>
				</big>
				<div id="Description">
					<FormattedText>{data.detailedDescription}</FormattedText>
				</div>
			</Fragment>
		}>
			<section hidden={is_empty(data.templateParameters)}>
				<h2>Template Parameters</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{data.templateParameters?.map((x, i) => {
							return (
								<tr key={i}>
									<td> {x.name} </td>
									<td>
										<FormattedText>{x.description}</FormattedText>
									</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>
			
			<section hidden={is_empty(data.memberGroups) && !data.hasOwnProperty("constructor")} id="MemberFunctions">
				<h2>Members</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						<tr hidden={!data.hasOwnProperty("constructor")}>
							<td>
								<Reference path={`/api/${data.project}/${data.header}/${betterEncodeURIComponent(data.name)}/${betterEncodeURIComponent(data.name)}`}>
									(constructors)
								</Reference>
							</td>
							<td>
								{data.constructor?.briefDescription}
							</td>
						</tr>
						{data.memberGroups?.map((x, i) => {
							return (
								<Fragment key={i}>
									<tr>
										<td colSpan="2" hidden={x.name === undefined && x.description === undefined}>
											<h3>{x.name}</h3>
											<div>
												<FormattedText>{x.description}</FormattedText>
											</div>
										</td>
									</tr>
									{x.items.map((x, i) => {
										return (
											<tr key={i}>
												<td>
													<MemberReference name={x.name} />
												</td>
												<td>
													<FormattedText>{x.briefDescription}</FormattedText>
												</td>
											</tr>
										)
									})}
								</Fragment>
							)
						})}
					</tbody>
				</table>
			</section>
		</BaseEntityViewer>
	)
}