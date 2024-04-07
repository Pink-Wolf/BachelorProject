import Style from "@/styles/EntityViewer.css";

import Link from "next/link";
import { Fragment } from "react";
import { CodeViewer, Declaration, Reference } from "./CodeComponents";

export default function ClassViewer(props) {
	const data = props.data;
	if (data == undefined) return <div/>

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	function MemberReference(props) {
		return (
			<Reference path={`${data.project}/${data.header}/${data.name}/${props.name}`}>
				{props.name}
			</Reference>
		)
	}
	
	return (
		<section>
			<h1 id="Path">
				{`${data.namespace}::${data.name}`}
			</h1>
			<small>
				<p id="Project">In project <Reference path={data.project}>{data.project}</Reference></p>
				<p id="Header">Defined in header <Reference path={`${data.project}/${data.header}`}>&lt;{data.header}&gt;</Reference></p>
			</small>
			<big id="Declaration">
				<Declaration>{data.declaration}</Declaration>
			</big>
			<p id="Description">
				{data.detailedDescription}
			</p>

			<section hidden={is_empty(data.warnings)} className="warning" id="Warnings">
				<h2>Warning</h2>
				{data.warnings?.map((x, i) => {
					return <blockquote key={i}>{x}</blockquote>
				})}
			</section>

			<section hidden={is_empty(data.dataMembers)} id="DataMembers">
				<h2>Data Members</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{data.dataMembers?.map((x, i) => {
							return (
								<tr key={i}>
									<td>
										<MemberReference name={x.name} />
									</td>
									<td>{x.description}</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>

			<section hidden={is_empty(data.methodGroups) && !data.constructor} id="MemberFunctions">
				<h2>Member Functions</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						<tr hidden={!data.constructor}>
							<td colSpan="2">
								<Reference path={`${data.project}/${data.header}/${data.name}/${data.name}`}>
									(constructors)
								</Reference>
							</td>
						</tr>
						{data.methodGroups?.map((x, i) => {
							return (
								<Fragment key={i}>
									<tr>
										<td colSpan="2">
											<h3>{x.name}</h3>
											<p>{x.description}</p>
										</td>
									</tr>
									{x.items.map((x, i) => {
										return (
											<tr key={i}>
												<td>
													<MemberReference name={x.name} />
												</td>
												<td>{x.briefDescription}</td>
											</tr>
										)
									})}
								</Fragment>
							)
						})}
					</tbody>
				</table>
			</section>

			<section hidden={!data.example} id="Example">
				<h2>Example</h2>
				<p>{data.example?.description}</p>
				<CodeViewer>
					{data.example?.code}
				</CodeViewer>
			</section>

			<section hidden={is_empty(data.related)} id="Related">
				<h2>Related</h2>
				{data.related?.map((x, i) => {
					return (
						<p key={i}>
							<Reference link={x}>
								{x}
							</Reference>
						</p>
					)
				})}
			</section>

		</section>
	)
}